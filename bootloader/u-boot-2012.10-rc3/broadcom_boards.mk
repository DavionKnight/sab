#
#   Copyright (C) 2013, Broadcom Corporation. All Rights Reserved. 
#    
#   Permission to use, copy, modify, and/or distribute this software for any 
#   purpose with or without fee is hereby granted, provided that the above 
#   copyright notice and this permission notice appear in all copies. 
#    
#   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES 
#   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF 
#   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY 
#   SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
#   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION 
#   OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
#   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
#

#########################################################################
## Broadcom - support for incremental compiles
#########################################################################

fpga911140_config: $(obj)fpga911140_config 
$(obj)fpga911140_config: $(TOPDIR)/include/configs/fpga911140.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm911160_tablet_config: $(obj)bcm911160_tablet_config 
$(obj)bcm911160_tablet_config: $(TOPDIR)/include/configs/bcm911160_tablet.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm91116z_tablet_config: $(obj)bcm91116z_tablet_config 
$(obj)bcm91116z_tablet_config: $(TOPDIR)/include/configs/bcm91116z_tablet.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm91116o_tablet_config: $(obj)bcm91116o_tablet_config 
$(obj)bcm91116o_tablet_config: $(TOPDIR)/include/configs/bcm91116o_tablet.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm911160sv_config: $(obj)bcm911160sv_config 
$(obj)bcm911160sv_config: $(TOPDIR)/include/configs/bcm911160sv.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm911160sv_mpg_config: $(obj)bcm911160sv_mpg_config 
$(obj)bcm911160sv_mpg_config: $(TOPDIR)/include/configs/bcm911160sv_mpg.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm91116osv_config: $(obj)bcm91116osv_config 
$(obj)bcm91116osv_config: $(TOPDIR)/include/configs/bcm91116osv.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm911160sv_li_config: $(obj)bcm911160sv_li_config 
$(obj)bcm911160sv_li_config: $(TOPDIR)/include/configs/bcm911160sv_li.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

cp_tablet_config: $(obj)cp_tablet_config 
$(obj)cp_tablet_config: $(TOPDIR)/include/configs/cp_tablet.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm91007as_hr_config: $(obj)bcm91007as_hr_config 
$(obj)bcm91007as_hr_config: $(TOPDIR)/include/configs/bcm91007as_hr.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

iproc_em_config: $(obj)iproc_em_config 
$(obj)iproc_em_config: $(TOPDIR)/include/configs/iproc_em.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_config: $(obj)bcm95301x_svk_config 
$(obj)bcm95301x_svk_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_config: $(obj)bcm95301x_svk_iproc_config 
$(obj)bcm95301x_svk_iproc_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_dual_core_config: $(obj)bcm95301x_svk_iproc_dual_core_config 
$(obj)bcm95301x_svk_iproc_dual_core_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_perf_config: $(obj)bcm95301x_svk_iproc_perf_config 
$(obj)bcm95301x_svk_iproc_perf_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_diag_config: $(obj)bcm95301x_svk_iproc_diag_config 
$(obj)bcm95301x_svk_iproc_diag_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_nand_spl_config: $(obj)bcm95301x_svk_iproc_nand_spl_config 
$(obj)bcm95301x_svk_iproc_nand_spl_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_nand_config: $(obj)bcm95301x_svk_iproc_nand_config 
$(obj)bcm95301x_svk_iproc_nand_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_ddr775_config: $(obj)bcm95301x_svk_iproc_ddr775_config 
$(obj)bcm95301x_svk_iproc_ddr775_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@
#DDR-EYE-SHMOO: added
bcm95301x_svk_iproc_ddr775_rw_eye_config: $(obj)bcm95301x_svk_iproc_ddr775_rw_eye_config 
$(obj)bcm95301x_svk_iproc_ddr775_rw_eye_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_iproc_ddr775_rw_eye_pwm_config: $(obj)bcm95301x_svk_iproc_ddr775_rw_eye_pwm_config 
$(obj)bcm95301x_svk_iproc_ddr775_rw_eye_pwm_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

bcm95301x_svk_l2cram_config: $(obj)bcm95301x_svk_l2cram_config 
$(obj)bcm95301x_svk_l2cram_config: $(TOPDIR)/include/configs/bcm95301x_svk.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

helix4_emul_config: $(obj)helix4_emul_config 
$(obj)helix4_emul_config: $(TOPDIR)/include/configs/helix4_emul.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

helix4_config: $(obj)helix4_config 
$(obj)helix4_config: $(TOPDIR)/include/configs/helix4.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

helix4_nand_config: $(obj)helix4_nand_config 
$(obj)helix4_nand_config: $(TOPDIR)/include/configs/helix4.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

helix4_nand_spl_config: $(obj)helix4_nand_spl_config 
$(obj)helix4_nand_spl_config: $(TOPDIR)/include/configs/helix4.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

helix4_be_config: $(obj)helix4_be_config 
$(obj)helix4_be_config: $(TOPDIR)/include/configs/helix4.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

helix4_nand_be_config: $(obj)helix4_nand_be_config 
$(obj)helix4_nand_be_config: $(TOPDIR)/include/configs/helix4.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_emul_config: $(obj)northstar_plus_emul_config 
$(obj)northstar_plus_emul_config: $(TOPDIR)/include/configs/northstar_plus_emul.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_config: $(obj)northstar_plus_config 
$(obj)northstar_plus_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_32_config: $(obj)northstar_plus_32_config 
$(obj)northstar_plus_32_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_stdk_config: $(obj)northstar_plus_stdk_config 
$(obj)northstar_plus_stdk_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_nand_config: $(obj)northstar_plus_nand_config 
$(obj)northstar_plus_nand_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@
	
northstar_plus_diag_config: $(obj)northstar_plus_diag_config 
$(obj)northstar_plus_diag_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_diag_svk_mfg_config: $(obj)northstar_plus_diag_svk_mfg_config 
$(obj)northstar_plus_diag_svk_mfg_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

northstar_plus_mdk_config: $(obj)northstar_plus_mdk_config 
$(obj)northstar_plus_mdk_config: $(TOPDIR)/include/configs/northstar_plus.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_emul_config: $(obj)hurricane2_emul_config 
$(obj)hurricane2_emul_config: $(TOPDIR)/include/configs/hurricane2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_config: $(obj)hurricane2_config 
$(obj)hurricane2_config: $(TOPDIR)/include/configs/hurricane2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_nand_config: $(obj)hurricane2_nand_config 
$(obj)hurricane2_nand_config: $(TOPDIR)/include/configs/hurricane2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_nor_config: $(obj)hurricane2_nor_config
$(obj)hurricane2_nor_config: $(TOPDIR)/include/configs/hurricane2.h
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_noshmoo_config: $(obj)hurricane2_noshmoo_config 
$(obj)hurricane2_noshmoo_config: $(TOPDIR)/include/configs/hurricane2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane2_rom_emul_config: $(obj)hurricane2_rom_emul_config 
$(obj)hurricane2_rom_emul_config: $(TOPDIR)/include/configs/hurricane2_rom_emul.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

katana2_emul_config: $(obj)katana2_emul_config 
$(obj)katana2_emul_config: $(TOPDIR)/include/configs/katana2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

katana2_config: $(obj)katana2_config 
$(obj)katana2_config: $(TOPDIR)/include/configs/katana2.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

katana2_nand_config: $(obj)katana2_nand_config 
$(obj)katana2_nand_config: $(TOPDIR)/include/configs/katana2.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

katana2_nand_spl_config: $(obj)katana2_nand_spl_config 
$(obj)katana2_nand_spl_config: $(TOPDIR)/include/configs/katana2.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

katana2_be_config: $(obj)katana2_be_config 
$(obj)katana2_be_config: $(TOPDIR)/include/configs/katana2.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

katana2_nand_be_config: $(obj)katana2_nand_be_config 
$(obj)katana2_nand_be_config: $(TOPDIR)/include/configs/katana2.h  
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_config: $(obj)greyhound_config 
$(obj)greyhound_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_nand_config: $(obj)greyhound_nand_config 
$(obj)greyhound_nand_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_nor_config: $(obj)greyhound_nor_config 
$(obj)greyhound_nor_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_ref_config: $(obj)greyhound_ref_config 
$(obj)greyhound_ref_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_stdk_config: $(obj)greyhound_stdk_config 
$(obj)greyhound_stdk_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_be_config: $(obj)greyhound_be_config 
$(obj)greyhound_be_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_nand_be_config: $(obj)greyhound_nand_be_config 
$(obj)greyhound_nand_be_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_nor_be_config: $(obj)greyhound_nor_be_config 
$(obj)greyhound_nor_be_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_ref_be_config: $(obj)greyhound_ref_be_config 
$(obj)greyhound_ref_be_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

greyhound_stdk_be_config: $(obj)greyhound_stdk_be_config 
$(obj)greyhound_stdk_be_config: $(TOPDIR)/include/configs/greyhound.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

cygnus_emul_config: $(obj)cygnus_emul_config
$(obj)cygnus_emul_config: $(TOPDIR)/include/configs/cygnus_emul.h
	@$(MKCONFIG) -A $(@F:_config=); touch $@

cygnus_diag_config: $(obj)cygnus_diag_config
$(obj)cygnus_diag_config: $(TOPDIR)/include/configs/cygnus.h
	@$(MKCONFIG) -A $(@F:_config=); touch $@

saber2_config: $(obj)saber2_config 
$(obj)saber2_config: $(TOPDIR)/include/configs/saber2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

saber2_nand_config: $(obj)saber2_nand_config 
$(obj)saber2_nand_config: $(TOPDIR)/include/configs/saber2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

saber2_nand_spl_config: $(obj)saber2_nand_spl_config 
$(obj)saber2_nand_spl_config: $(TOPDIR)/include/configs/saber2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

saber2_stdk_config: $(obj)saber2_stdk_config
$(obj)saber2_stdk_config: $(TOPDIR)/include/configs/saber2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

saber2_be_config: $(obj)saber2_be_config 
$(obj)saber2_be_config: $(TOPDIR)/include/configs/saber2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

saber2_nand_be_config: $(obj)saber2_nand_be_config 
$(obj)saber2_nand_be_config: $(TOPDIR)/include/configs/saber2.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

saber2_stdk_be_config: $(obj)saber2_stdk_be_config
$(obj)saber2_stdk_be_config: $(TOPDIR)/include/configs/saber2.h
	@$(MKCONFIG) -A $(@F:_config=); touch $@

cygnus_config: $(obj)cygnus_config
$(obj)cygnus_config: $(TOPDIR)/include/configs/cygnus.h
	@$(MKCONFIG) -A $(@F:_config=); touch $@

cygnus_nand_config: $(obj)cygnus_nand_config
$(obj)cygnus_nand_config: $(TOPDIR)/include/configs/cygnus.h
	@$(MKCONFIG) -A $(@F:_config=); touch $@

cygnus_tzone_config: $(obj)cygnus_tzone_config
$(obj)cygnus_tzone_config: $(TOPDIR)/include/configs/cygnus.h
	@$(MKCONFIG) -A $(@F:_config=); touch $@
	
hurricane3_config: $(obj)hurricane3_config 
$(obj)hurricane3_config: $(TOPDIR)/include/configs/hurricane3.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane3_nand_config: $(obj)hurricane3_nand_config 
$(obj)hurricane3_nand_config: $(TOPDIR)/include/configs/hurricane3.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane3_nor_config: $(obj)hurricane3_nor_config 
$(obj)hurricane3_nor_config: $(TOPDIR)/include/configs/hurricane3.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@

hurricane3_stdk_config: $(obj)hurricane3_stdk_config
$(obj)hurricane3_stdk_config: $(TOPDIR)/include/configs/hurricane3.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@


hurricane3_emu_config: $(obj)hurricane3_emu_config
$(obj)hurricane3_emu_config: $(TOPDIR)/include/configs/hurricane3.h 
	@$(MKCONFIG) -A $(@F:_config=); touch $@
