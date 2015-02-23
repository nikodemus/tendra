/* $Id$ */

/*
 * Copyright 2011-2012, The TenDRA Project.
 * Copyright 1997, United Kingdom Secretary of State for Defence.
 * Copyright 1993, Open Software Foundation, Inc.
 *
 * See doc/copyright/ for the full copyright terms.
 */

/*
 *  Translation is controlled by this module.
 *  Code generation follows the following phases:
 *
 *  1. The TDF is read in, applying bottom-up optimisations.
 *  2. Top-down optimisations are performed.
 *  3. Register allocation is performed, and TDF idents introduced
 *     so code generation can be performed with no register spills.
 *  4. Code is generated for each procedure, and global declarations processed.
 *  5. Peephole optimisation and instruction scheduling.
 *     (not implemented yet)
 *
 *  In a little more detail:
 *
 *  1) During the TDF reading process for tag declarations and tag
 *  definitions, applications of tokens are expanded as they are
 *  encountered, using the token definitions.  Every token used must have
 *  been previously defined in the bitstream.
 *
 *  The reading of the tag definitions creates a data structure in memory
 *  which directly represents the TDF.  At present, all the tag definitions
 *  are read into memory in this way before any further translation is
 *  performed.  This will shortly be changed, so that translation is
 *  performed in smaller units.  The translator is set up already so that
 *  the change to translate in units of single global definitions (or
 *  groups of these) can easily be made.
 *
 *  During the creation of the data structure bottom-up optimisations
 *  are performed.  These are the optimisations which can be done when a
 *  complete sub-tree of TDF is present, and are independent of the context
 *  in which the sub-tree is used.  They are defined in refactor.c and
 *  refactor_id.c.  These optimisation do such things as use the commutative
 *  and associative laws for plus to collect together and evaluate
 *  constants.  More ambitious examples of these bottom-up optimisations
 *  include all constant evaluation, elimination of inaccessible code, and
 *  forward propagation of assignments of constants.
 *
 *  2) After reading in the TDF various optimisations are performed which
 *  cannot be done until the whole context is present.  For example,
 *  constants cannot be extracted from a loop when we just have the loop
 *  itself, because we cannot tell whether the variables used in it are
 *  aliased.
 *
 *  These optimisations are invoked by opt_all_exps which is defined in
 *  indep.c.  They include extraction of constants from loops, common
 *  expression elimination on them and strength reduction for indexing.
 *
 *  3) Allocatable registers are partitioned into two sets, the s regs
 *  which are preserved over calls, and the t regs which are not.
 *
 *  The TDF is scanned introducing TDF idents so that expressions can be
 *  evaluated within the available t regs with no spills.  These new idents
 *  may be later allocated to a s reg later, if the weighting algorithm
 *  (below) considers this worth while.  Otherwise they will be on the stack.
 *
 *  Information is collected to help in global register allocation.  During
 *  a forward recursive scan of the TDF the number of accesses to each
 *  variable is computed (making assumptions about the frequency of
 *  execution of loops).  Then on the return scan of this recursion, for
 *  each declaration, the number of registers which must be free for it to
 *  be worthwhile to use a register is computed, and put into the TDF as
 *  the "break" point.  The procedures to do this are defined in weights.c.
 *
 *  Suitable idents not alive over a procedure call are allocated to a t reg,
 *  and others to s regs.  At the same time stack space requirements are
 *  calculated, so this is known before code for a procedure is generated.
 *
 *  4) Finally the code is generated without register spills.  The code is
 *  generated by make_code() in make_code.c, and make_XXX_code() in proc.c.
 *
 *  Note that loop unrolling optimisations are not currently implemented.
 *  Library procedures such as memcpy() are not treated specially.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <shared/check.h>
#include <shared/error.h>
#include <shared/xalloc.h>

#include <reader/externs.h>		/* for "inits.h" init_XXX() procs */
#include <reader/basicread.h>
#include <reader/main_reads.h>
#include <reader/readglob.h>

#include <construct/installglob.h>
#include <construct/exp.h>

#include <main/driver.h>
#include <main/flags.h>
#include <main/print.h>

#include <refactor/optimise.h>

#include "memtdf.h"
#include "codegen.h"
#include "tempdecs.h"
#include "weights.h"
#include "procrecs.h"
#include "regalloc.h"
#include "make_code.h"
#include "eval.h"
#include "scan.h"
#include "stabs_diag3.h"
#include "translate.h"
#include "stack.h"
#include "frames.h"
#include "macro.h"
#include "dynamic_init.h"
#include "stack.h"
#include "localexpmacs.h"

int maxfix_tregs;		/* The number of t regs allocatable */
dec **main_globals;		/* The globals decs array */
procrec *procrecs;		/* The proc records array */
dec * diag_def = NULL;
bool environ_externed=0;/* environ bug work around */
long total_no_of_globals = 0;
bool done_scan = 0;

/*
 * Translate a TDF capsule 
 */
void translate_capsule(void)
{
  int noprocs, noglobals;
  int procno, globalno;
  dec *crt_def;
  space tempregs;
  int r;
  bool anydone;

  /*
   * Do the high level, portable, TDF optimisations 
   */
  opt_all_exps();

  /*
   * Initialise diagnostic information and produce stab for basic types.
   */
  if (diag != DIAG_NONE)
  {
    init_diag();
  }

	if (do_macros) {
		init_macros();
	}

	asm_label( "L.TDF.translated"); /* XXX: unneccessary */
	asm_comment( "powertrans");


  /*
   * Generate .extern, .globl, .lglobl, .comm, .lcomm.
   * Also take the opportunity to count proc and global definitions.
   *
   * Note that .lglobl is only generated if diag is set (from -g).
   * It requires an updated IBM assembler with fix:
   * IX23435 Assembler can't create C_HIDEXT class for static names
   */

  noprocs = 0;
  noglobals = 0;

  for (crt_def = top_def; crt_def != NULL; crt_def = crt_def->def_next)
  {
    exp tg = crt_def->dec_exp;
    shape s = crt_def->dec_shape;
    bool extnamed = crt_def->extnamed;
    char *id;

    noglobals++;
    /* diag_def needed for find_dd in stabs_diag3.c */
    diag_def=crt_def;

    if (diag != DIAG_NONE)
    {
      /*
       * It is safe to fixup all names here.  C static within procs
       * do not get a diag_descriptor so fixup_name does not change
       * their names.
       */
      fixup_name(son(tg), top_def, crt_def);
    }

    id = crt_def->dec_id;		/* might be changed by fixup_name() */

    asm_comment("%s: extnamed=%d no(tg)=%d isvar(tg)=%d", id, extnamed, no(tg), isvar(tg));
    asm_comment("\tname(tg)=%d dec_outermost=%d have_def=%d son(tg)!=NULL=%d",
		name(tg), crt_def->dec_outermost, crt_def->have_def, son(tg) != NULL);
    if (son(tg) != NULL)
      asm_comment("\tdec_shape, sh(tg), sh(son(tg))=%d,%d,%d", name(s), name(sh(tg)), name(sh(son(tg))));

    crt_def->have_def = (son(tg)!=NULL);

    assert(name(tg) == ident_tag);
    assert(son(tg) == NULL || name(sh(tg)) == name(s));

    if (son(tg) == NULL)
    {
#if 0
      if (diag == DIAG_NONE && no(tg) == 0)
#else
      if(no(tg)==0)/* only put out an extern instruction if there is a use */
#endif
	 {
	/* no use of this tag, do nothing */
      }
      else if (extnamed)
      {
	if (name(s) == prokhd)
	{
	  asm_printop(".extern %s", id);	/* proc descriptor */
	  asm_printop(".extern .%s", id);	/* proc entry point */
	}
	else
	{
#if 1
	  if (strcmp(id, "environ") == 0)
	  {
	    /*
	     * Kludge for environ, .extern for .csect, AIX 3.1.5 ld/library bug maybe?
	     * /lib/syscalls.exp states that environ & errno are specially handled,
	     * located on the stack at fixed addresses.
	     */
	    asm_printop(".extern %s[RW]", id);
	    environ_externed=1;
	  }
	  else
#endif
	  {
	    asm_printop(".extern %s", id);
	  }
	}
      }
      else
      {
	long byte_size = ALIGNNEXT(shape_size(sh(son(tg))), 64) >> 3;
	/* +++ is .lcomm always kept double aligned?  Otherwise how do we do it? */

	assert(extnamed);
	asm_printop(".lcomm %s,%ld", id, byte_size);
      }
    }
    else if (IS_A_PROC(son(tg)))
    {
      noprocs++;

      if (extnamed)
      {
	asm_printop(".globl %s", id);		/* id proc descriptor */
	asm_printop(".globl .%s", id);	/* .id entry point */
      }
      else if (diag != DIAG_NONE)
      {
	/* .lglobl is not documented, but avoids dbx and gdb becoming confused */
	/* +++ always when .lglobl documented */
	asm_printop(".lglobl .%s", id);	/* .id entry point */
      }
    }
    else if (is_comm(son(tg)) && (diag != DIAG_NONE || extnamed || no(tg) > 0))
    {
      /* zero initialiser needed */
      long size = shape_size(sh(son(tg)));
      long align = shape_align(sh(son(tg)));
      long byte_size = ALIGNNEXT(size, 64) >> 3;	
      /* +++ do we need to round up? */
      int aligncode = ((align > 32 || size > 32) ? 3 : 2);
      /* +++ is .lcomm always kept double aligned?
       * Otherwise how do we do it? */

      /* assembler is happy with .comm of size 0, 
       * so no need to special case unknown size */

      if (extnamed)
      {
	asm_printop(".comm %s,%ld,%d", id, byte_size, aligncode);
	if (diag != DIAG_NONE)
	  diag3_driver->stab_global(diag_def->diag_info, son(tg), id, extnamed);
      }
      else
      {
	if (diag != DIAG_NONE)
	{
	  char *csect_name = "C.";

	  /*
	   * assembler is confused if it sees .stabx before any .csect,
	   * so keep it happy with a useless .csect:
	   */
	  asm_printop(".csect [PR]");
	  asm_printop(".lcomm %s,%ld,%s", id, byte_size, csect_name);
	  stab_bs(csect_name);
	  diag3_driver->stab_global(diag_def->diag_info, son(tg), id, extnamed);
	  stab_es(csect_name);
	}
	else if (no(tg) > 0)			/* used */
	{
	  asm_printop(".lcomm %s,%ld", id, byte_size);
	}
      }

      assert((align&63)==0 || align < 64);

      /* mark the defininition as processed */
      crt_def->processed = 1;
    }
    else
    {
      if (extnamed)
	asm_printop(".globl %s", id);
      else if (diag != DIAG_NONE)
	asm_printop(".lglobl %s", id);
      /* to avoid 'warning: global ignored' message from dbx */
    }
  }

  if (do_dynamic_init) {
    IGNORE do__main_extern();
  }

  if (do_profile)
#ifdef TDF_MCOUNT
    asm_printop(".extern .TDF_mcount");
#else
    asm_printop(".extern .mcount");
#endif


  /*
   * Alloc memory for procrecs array, info retained between phases
   * about procs and how parameters will be stored.
   */
  if (noprocs == 0) {
    procrecs = NULL;
  } else {
    procrecs = (procrec *) xcalloc(noprocs, sizeof (procrec));
  }

  /*
   * Alloc memory for main_globals, used to lookup assembler names.
   */
  if (noglobals == 0) {
    main_globals = NULL;
  } else {
    main_globals = (dec**)xcalloc(noglobals, sizeof(dec*));
  }

  /*
   * Generate .toc entries.
   * Also take opportunity to setup main_globals.
   */
  asm_printf( "\n\t.toc\n");

  for (crt_def = top_def; crt_def != NULL; crt_def = crt_def->def_next)
  {
    exp tg = crt_def->dec_exp;
    char *id = crt_def->dec_id;
    /* 
     * no(tg) is number of uses 
     * If tg is used in this module, 
     * generate a .toc entry so it can be addressed 
     * +++ differentiate proc descriptor/entry point usage 
     */
    if (no(tg) > 0 || strcmp(id,"__TDFhandler")==0 
	|| strcmp(id,"__TDFstacklim")==0)
    {
      bool extnamed = crt_def->extnamed;
      char *storage_class;

      if (extnamed && son(tg) == NULL)
      {
	/* extern from another module */
	if (name(crt_def->dec_shape) == prokhd)
	  storage_class = "";	/* proc descriptor */
	else
	  storage_class = "";	/* unknown data */
      }
      else
      {
	storage_class = "";		/* this module */
      }
#if 1
      if (strcmp(id, "environ") == 0 && environ_externed )
      {
	/* kludge for environ, .extern for .csect, IBM ld/library bug maybe? */
	storage_class = "[RW]";

      }
#endif
      asm_printf( "T.%s:\t.tc\t%s[TC],%s%s\n", id, id, id, storage_class);
    }
  }



  /* number proc defs and setup main_globals */
  procno = 0;
  globalno = 0;
  for (crt_def = top_def; crt_def != NULL; crt_def = crt_def->def_next)
  {
    exp tg = crt_def->dec_exp;

    main_globals[globalno] = crt_def;
    crt_def->sym_number = globalno;
    globalno++;

    if (son(tg) != NULL && IS_A_PROC(son(tg)))
    {
      no(son(tg)) = procno;	/* index into procrecs in no(proc) */
      procno++;
    }
  }

  assert(procno==noprocs);
  assert(globalno==noglobals);
  total_no_of_globals=globalno;
  
 /*
   * Scan to put proc bodies in POWER form,
   * and calculate register and stack space needs.
   */

  /*
   * First work out which t fixed point regs, those not preserved over calls,
   * can be used.  This needs to be done before scan() which adds idents
   * so temp reg needs are within available temp reg set.
   */

  /* initial reg sets */
  tempregs.fixed = PROC_TREGS;
  tempregs.flt = PROC_FLT_TREGS;

  /* ensure R_TMP0 not allocatable */
  tempregs.fixed |= RMASK(R_TMP0);

  /* count t fixed point regs we can use, and set the global maxfix_tregs */
  maxfix_tregs = 0;
  for ( r = R_FIRST; r <= R_LAST; r++ )
  {
    /* bit clear means allocatable */
    if (IS_TREG(r) && (tempregs.fixed&RMASK(r)) == 0)
      maxfix_tregs++;
  }
  maxfix_tregs -= REGISTER_SAFETY_NUMBER;
  
  asm_comment("maxfix_tregs=%d(%#lx) maxfloat_tregs=%d(%#lx)",
	maxfix_tregs, (unsigned long) tempregs.fixed, MAXFLT_TREGS, (unsigned long) tempregs.flt);


  /*
   * Scan all the procs, to put the TDF in POWER form,
   * and do register allocation.
   */
  for (crt_def = top_def; crt_def != NULL; crt_def = crt_def->def_next)
  {
    exp tg = crt_def->dec_exp;

    if (son(tg) != NULL && IS_A_PROC(son(tg)))
    {
      procrec *pr = &procrecs[no(son(tg))];
      exp *st = &son(tg);
      int freefixed=MAXFIX_SREGS;/* The maximum no of free fixed s regs */
      int freefloat=MAXFLT_SREGS;/* The maximum no of free float s regs */
      int r;
          
      /* 
       * SCAN the procedure
       */

      pr->needsproc = scan(st, &st);
      set_up_frame_pointer(pr,son(tg));
      /*
       * WEIGHTS
       * estimate usage of tags in body of proc,
       * calculating the break points for register allocation
       */
      if (!(pr->save_all_sregs))
      {
	IGNORE weightsv(UNITWEIGHT, bro(son(son(tg))));      
      }
      /* Check to see if we need a frame pointer */
      if (pr->has_fp)
      {
	freefixed--;
      }
      if(pr->has_tp)
      {
	freefixed--;
      }
      /* 
       * REGALLOC
       * reg and stack allocation for tags 
       */
      pr->spacereqproc = regalloc(bro(son(son(tg))), freefixed, freefloat, 0);
      /* 
       * Ensure that the registers that were not allocated get stored
       */
      for ( r=freefixed+R_13 ; r <= R_31 ; r++)
      {
	pr->spacereqproc.fixdump = pr->spacereqproc.fixdump | RMASK(r);
      }
      if (pr->save_all_sregs)
      {
	pr->spacereqproc.fixdump = 0xffffe000;
	pr->spacereqproc.fltdump = 0xffffc000;
      }
      set_up_frame_info(pr,son(tg));
    }
  }
  done_scan = 1;
  
  /*
   * Evaluate outer level data initialisers in [RW] section.
   */
  anydone = 0;
  for (crt_def = top_def; crt_def != NULL; crt_def = crt_def->def_next)
  {
    exp tg = crt_def->dec_exp;
    char *id = crt_def->dec_id;
    bool extnamed = crt_def->extnamed;
    diag_def=crt_def;/* just in case find_dd is called */
    asm_comment("no(tg)=%d isvar(tg)=%d extnamed=%d son(tg)==NULL=%d",
		 no(tg), isvar(tg), extnamed, son(tg)==NULL);
    if (son(tg) != NULL)
    {
      /*
       * Skip if already processed, eg identified as is_comm() 
       */
      if (crt_def->processed)
	continue;
      /*
       * Skip if zero uses and internal to module 
       * unless generating diagnostics 
       */
      if (!(diag != DIAG_NONE || extnamed || no(tg) > 0))
	continue;
      /* +++ could do better than making everything except strings [RW] */
      if ( ! IS_A_PROC(son(tg)) ) 
	/* put all things in [RW] section */
      {
	/* 
	 * Non proc, which is isvar() [variable] for [RW] section 
	 */
	long symdef = crt_def->sym_number;
	
	/* Check to see if we have made any entries yet */
	if (!anydone)
	{
	  anydone = 1;
	  asm_printf( "\n\t.csect\tW.[RW]\n");
	  if (diag != DIAG_NONE)
	  {
	    stab_bs("W.[RW]");
	  }
	}

	evaluated(son(tg), -symdef - 1);

	if (diag != DIAG_NONE)
	{
	  diag3_driver->stab_global(diag_def->diag_info, son(tg), id, extnamed);
	}
	asm_printf( "#\t.enddata\t%s\n\n", id);

	/* mark the defininition as processed */
	crt_def->processed = 1;
      }
    }
  }
  if (diag != DIAG_NONE && anydone)
  {
    stab_es("W.[RW]"); /* Close the RW section stab */
  }
  


  /*
   * Evaluate outer level data initialisers in [RO] section.
   */
  anydone = 0;			/* set to 1 after first tag output */

  for (crt_def = top_def; crt_def != NULL; crt_def = crt_def->def_next)
  {
    exp tg = crt_def->dec_exp;
    char *id = crt_def->dec_id;
    bool extnamed = crt_def->extnamed;
    diag_def=crt_def;/* just in case find_dd is called */
    if (son(tg) != NULL)
    {
      /* skip if already processed, eg identified as is_comm() */
      if (crt_def->processed)
	continue;

      /* 
       * Skip if zero uses and internal to module unless 
       * generating diagnostics 
       */
      if (!(diag != DIAG_NONE || extnamed || no(tg) > 0))
	continue;

      if (!IS_A_PROC(son(tg)))
      {
	/* non proc, which is not isvar() [variable] for [RO] section */
	long symdef = crt_def->sym_number;

	if (!anydone)
	{
	  anydone = 1;
	  asm_printf( "\n\t.csect\tR.[RO]\n");
	  if (diag != DIAG_NONE)
	  {
	    stab_bs("R.[RO]");
	  }
	}

	evaluated(son(tg), symdef + 1);

	if (diag != DIAG_NONE)
	{
	  diag3_driver->stab_global(diag_def->diag_info, son(tg), id, extnamed);
	}
	asm_printf( "#\t.enddata\t%s\n\n", id);

	/* mark the defininition as processed */
	crt_def->processed = 1;
      }
    }
  }

  if (diag != DIAG_NONE && anydone)
  {
    stab_es("R.[RO]");
  }
  anydone=0;
  
  /*
   * Translate procedures.
   */
  for (crt_def = top_def; crt_def != NULL; crt_def = crt_def->def_next)
  {
    exp tg = crt_def->dec_exp;
    char *id = crt_def->dec_id;
    bool extnamed = crt_def->extnamed;
    
    if (son(tg) != NULL)
    {
      /* skip if already processed */
      if (crt_def->processed)
	continue;

      /* skip if zero uses and internal to module unless generating diagnostics */
      if (!(diag != DIAG_NONE || extnamed || no(tg) > 0))
	continue;

      if (IS_A_PROC(son(tg)))
      {
	/* translate code for proc */
	asm_printf( "\n");		/* make proc more visable to reader */
	diag_def=crt_def;
	/* switch to correct file */
	if (diag != DIAG_NONE && diag_def->diag_info!=NULL )
	{
	  anydone=1;
	  stab_proc1(son(tg), id, extnamed);
	}
	

	asm_printf( "#\t.proc\n");

	/* generate descriptor */
	asm_printop(".csect [DS]");
	asm_label( "%s", id);
	asm_printop(".long .%s,TOC[tc0],0", id);

	/* generate code */
	asm_printop(".csect [PR]");
	asm_label( ".%s", id);

	/* stab proc details */
	if (diag != DIAG_NONE && diag_def->diag_info!=NULL)
	{
	  stab_proc2(son(tg), id, extnamed);
	}
	
	seed_label();		/* reset label sequence */
	settempregs(son(tg));	/* reset getreg sequence */

	code_here(son(tg), tempregs, nowhere);

	if (diag != DIAG_NONE && diag_def->diag_info!=NULL)
	{
	  stab_endproc(son(tg), id, extnamed);
	}
	
	asm_printf( "#\t.end\t%s\n", id);

	/* mark the defininition as processed */
	crt_def->processed = 1;
      }
    }
  }
  if ( diag != DIAG_NONE && anydone )
  {
    stab_end_file();/* Ties up any open .bi's with .ei's */
  }

}


baseoff find_tg(char *n)
{
  int i;
  exp tg;
  for (i = 0; i < total_no_of_globals; i++) {
    char *id = main_globals[i] -> dec_id;
    tg = main_globals[i] -> dec_exp;
    if (strcmp(id, n) == 0) return boff(tg);
  }
  printf("%s\n", n);
  error(ERR_SERIOUS, "Extension name not declared ");
  tg = main_globals[0] -> dec_exp;
  return boff(tg);
}
