# makefile for barotropic weather model

OFILES = 	angmom_bv.o archwr_bv.o ass.o cardin_bv.o datain_bv.o \
		energy_bv.o frsymm_bv.o frtosp_bv.o gaussg_bv.o \
		inital_bv.o initpv_bv.o keepnh_bv.o lgndre_bv.o \
		linear_bv.o linbal_bv.o main_bv.o nonlin_bv.o \
		ordleg.o specam_bv.o sptofr_bv.o tstep_bv.o \
		vduvjm_bv.o vertig_bv.o rdmtns_bv.o xlin_bv.o print1_bv.o \
		\
		initfft.o mdfftm.o mdfftg.o passg.o passm.o factr4.o

COFILES = timesf.o

model_bv: $(OFILES) bv_dec param_dec $(COFILES)
	f77 -O -f68881 -o a.out $(OFILES) $(COFILES)

timesf.o: timesf.c
	cc -c timesf.c

angmom_bv.o:  angmom_bv.f param_dec bv_dec

archwr_bv.o:  archwr_bv.f param_dec bv_dec

cardin_bv.o:  cardin_bv.f param_dec bv_dec

datain_bv.o:  datain_bv.f param_dec bv_dec

energy_bv.o:  energy_bv.f param_dec bv_dec
  
frsymm_bv.o:  frsymm_bv.f param_dec bv_dec

frtosp_bv.o:  frtosp_bv.f param_dec bv_dec

gaussg_bv.o:  gaussg_bv.f param_dec bv_dec

inital_bv.o:  inital_bv.f param_dec bv_dec

initpv_bv.o:  initpv_bv.f param_dec bv_dec

keepnh_bv.o:  keepnh_bv.f param_dec bv_dec

lgndre_bv.o:  lgndre_bv.f param_dec bv_dec

linear_bv.o:  linear_bv.f param_dec bv_dec

linbal_bv.o:  linbal_bv.f param_dec bv_dec

main_bv.o:    main_bv.f param_dec bv_dec

nonlin_bv.o:  nonlin_bv.f param_dec bv_dec

print1_bv.o:  print1_bv.f param_dec bv_dec

specam_bv.o:  specam_bv.f param_dec bv_dec

sptofr_bv.o:  sptofr_bv.f param_dec bv_dec

tstep_bv.o:   tstep_bv.f param_dec bv_dec

vertig_bv.o:  vertig_bv.f param_dec bv_dec

vduvjm_bv.o:  vduvjm_bv.f param_dec bv_dec

xlin_bv.o:    xlin_bv.f param_dec bv_dec

# the following are the fft library

initfft.o:	initfft.f

mdfftm.o:	mdfftm.f

mdfftg.o:	mdfftg.f

passg.o:	passg.f

passm.o:	passm.f

factr4.o:	factr4.f

clean:
	rm -f $(OFILES) model_bv
