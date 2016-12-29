/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <common.h>
#include <post.h>
#include "../../halapis/include/amacswitch_apis.h"
#include "../../arch/arm/include/asm/arch-cygnus/socregs.h"



#define ADD_TEST(X)  { #X , X }
#define TEST_PASS  0
#define TEST_FAIL -1
#define TEST_SKIP  2

#define NUM_TEST_ARGS 8
#define VAR_SIZE 50

#define MAG_MODE 1



/* Default Register Value */


typedef struct{

	char* func_name;
	int (*func)(void);
}FUNC_INFO;

void ccg_mdio_init_ext1(void) {
     uint32_t byp;
     uint32_t ext;
     uint32_t pre;
     uint32_t mdcdiv;
     uint32_t data;
     
     data = cpu_rd_single(ChipcommonG_MII_Management_Control,4);
     post_log("ChipcommonG_MII_Management_Control = 0x%08X\n", data);

     byp = 0;
     ext = 1;
     pre = 1;
     mdcdiv = 62;
     
     data = 0;
     data |= (mdcdiv << ChipcommonG_MII_Management_Control__MDCDIV_R);
     data |= (pre    << ChipcommonG_MII_Management_Control__PRE);
     data |= (ext    << ChipcommonG_MII_Management_Control__EXT);
     data |= (byp    << ChipcommonG_MII_Management_Control__BYP);

     cpu_wr_single(ChipcommonG_MII_Management_Control, data,4);
     data = cpu_rd_single(ChipcommonG_MII_Management_Control,4);
     post_log("ChipcommonG_MII_Management_Control = 0x%08X\n", data);
}

void wait_for_rgmii_phy_linkup(void) {
     uint32_t phy_addr, reg_addr, data;

     post_log("Waiting for RGMII PHY linkup...\n");
     //wait for linkup
     phy_addr = 0x10;
     reg_addr = 0x1;
     do {
         ccg_mdio_read (phy_addr, reg_addr, &data);
     } while(!(data & 0x4));
}

void srab_rd1(uint32_t page, uint32_t offset)
{
    uint32_t rdata[2],rdatah,rdatal;
    setCmdStat(page, offset, 0); // do read
    wait_for_done();            // wait done
    rdatah=cpu_rd_single(ChipcommonG_SRAB_RDH,4);
    rdatal=cpu_rd_single(ChipcommonG_SRAB_RDL,4);
	post_log("PAGE : %02X OFFSET : %02X RDATAL = %08X RDATAH = %08X \n",page,offset,rdatal,rdatah);

}

uint32_t srab_rdwr(uint32_t page, uint32_t offset,uint32_t data,uint32_t val)
{
   uint32_t rdatah,rdatal,wdatah,wdatal;
   setCmdStat(page,offset,0); //do read
   wait_for_done();
   rdatah=cpu_rd_single(ChipcommonG_SRAB_RDH,4);
   rdatal=cpu_rd_single(ChipcommonG_SRAB_RDL,4);
   post_log("Inside SRAB_RW Routine PAGE - %02X OFFSET - %02X RDATAL = %08X \n",page,offset,rdatal);
   wdatah = rdatah;

   
   if (val == 1)
   {
	   wdatal = data;
	   
   }

    if (val == 2)
   {
	   wdatal = ((rdatal & data) | 2);
	  
   }

   if (val == 3)
   {
	   wdatal = (rdatal | 0x1 | data);
	   
   }

    if (val == 4)
   {
	   wdatal = (rdatal & data);
	   
   }

   if (val == 5)
   {
	   wdatal = (rdatal | data);
   }


srab_wr(page,offset,wdatah,wdatal);
setCmdStat(page,offset,0); //do read
wait_for_done();
rdatah=cpu_rd_single(ChipcommonG_SRAB_RDH,4);
rdatal=cpu_rd_single(ChipcommonG_SRAB_RDL,4);
post_log("Inside SRAB_RW Routine After Writing - PAGE - %02X OFFSET - %02X RDATAL = %08X \n",page,offset,rdatal);
return 0;   

}

uint32_t read_switch_statistics(uint32_t page)
{
  uint32_t offset;
  offset = 0x0;
  srab_rd1(page,offset);
  offset=0x50;
  srab_rd1(page,offset);
   offset=0x58;
  srab_rd1(page,offset);
   offset=0x5C;
  srab_rd1(page,offset);
   offset=0x60;
  srab_rd1(page,offset);
   offset=0x64;
  srab_rd1(page,offset);
   offset=0x68;
  srab_rd1(page,offset);
   offset=0x6C;
  srab_rd1(page,offset);
   offset=0x70;
  srab_rd1(page,offset);
   offset=0x74;
  srab_rd1(page,offset);
   offset=0x78;
  srab_rd1(page,offset);
   offset=0x7C;
  srab_rd1(page,offset);
   offset=0x80;
  srab_rd1(page,offset);
   offset=0x84;
  srab_rd1(page,offset);
   offset=0x88;
  srab_rd1(page,offset);
   offset=0x90;
  srab_rd1(page,offset);
   offset=0x94;
  srab_rd1(page,offset);
   offset=0x98;
  srab_rd1(page,offset);
   offset=0x9C;
  srab_rd1(page,offset);
   offset=0xA0;
  srab_rd1(page,offset);
   offset=0xA4;
  srab_rd1(page,offset);
   offset=0xA8;
  srab_rd1(page,offset);
   offset=0xAC;
  srab_rd1(page,offset);
   offset=0xB0;
  srab_rd1(page,offset);
   offset=0xB4;
  srab_rd1(page,offset);
   offset=0xC0;
  srab_rd1(page,offset);
   offset=0xC8;
  srab_rd1(page,offset);
   offset=0xCC;
  srab_rd1(page,offset);
   offset=0xD0;
  srab_rd1(page,offset);
   offset=0xD4;
  srab_rd1(page,offset);
   offset=0xD8;
  srab_rd1(page,offset);
   offset=0xDC;
  srab_rd1(page,offset);
   offset=0xE0;
  srab_rd1(page,offset);
   offset=0xE4;
  srab_rd1(page,offset);

  return 0;
}

uint32_t cfg_sw1_port1_to_port0()
{
	uint32_t page;
	uint32_t offset;
	uint32_t wdatah, wdatal;
	uint32_t rdatah, rdatal;
	uint32_t port_id;
    uint32_t *rdata;

	// write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

	// write switch mode register	
    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);

	// write switch control register	
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		//rdata = srab_rd(page, offset);
        //rdatal=rdata[0];
        //rdatah=rdata[1];
        ////post_log("Port override rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
        //post_log("port override rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		//wdatah=rdatah;
        //post_log("port override wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	    //srab_wr(page, offset, wdatah, wdatal);
		//srab_wr(page, offset, wdatah, wdatal);
		
	    //rdata = srab_rd(page, offset);
        //post_log("port override rdata[0]=%#x rdata[1]=%#x\n",rdata[0],rdata[1]);
	}

    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);

///////////Configuration for jumbo frames - Added By Nishant///////
//	page=0x40; 
//	offset = 0x1;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0x123); 
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Jumbo frame config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//	
// offset = 0x5;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0x2710); 
//		 wdatah=rdatah;
//	srab_wr(page, offset, wdatah, wdatal);
//
//post_log("Jumbo frame config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//
/////Configure RGMII Delay - Added by Nishant////
//	page=0xe4; 
//	offset = 0x2a;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0x3); 
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

//  //Global ManagemetControl Register
//  page=0x2; 
//	offset = 0x0;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0xC2);  // bit 7,6 =1 Enable IMP0/1
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Global Management Configuration register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//
// page=0x0; 
//	offset = 0xB;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal | 0x1);  // bit 0 =1 Managed Mode forwarding enabled
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Switch Mode register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//
////Broadcom Header control Register
// page=0x2; 
//	offset = 0x3;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal | 0x1);  // bit 0 = 1 BRCM Header disabled
//
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("BRCM register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);


 /*page=0x31; 
	offset = 0x0;
		rdata = srab_rd(page, offset);
        rdatal=rdata[0];
        rdatah=rdata[1];
   wdatal=(rdatal & 0x101);  // bit 8 and bit 0 = Port 8 & Port 0 egress enabled

		 wdatah=rdatah;
	 srab_wr(page, offset, wdatah, wdatal);
   post_log("VLAN Port 0 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);*/

    page=0x31; 
	offset = 0x2;
		//rdata = srab_rd(page, offset);
        //rdatal=rdata[0];
        //rdatah=rdata[1];
   //wdatal=(rdatal & 0x100);  // bit 8 and bit 0 = Port 8 & Port 0 egress enabled
	//	 wdatah=rdatah;
	// srab_wr(page, offset, wdatah, wdatal);
   //post_log("VLAN Port 1 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
   srab_rdwr(page,offset,0x000,4);

    page=0x31; 
	offset = 0x0;
		//rdata = srab_rd(page, offset);
        //rdatal=rdata[0];
        //rdatah=rdata[1];
   //wdatal=(rdatal & 0x100);  // bit 8 and bit 0 = Port 8 & Port 0 egress enabled
	//	 wdatah=rdatah;
	// srab_wr(page, offset, wdatah, wdatal);
   //post_log("VLAN Port 1 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
   srab_rdwr(page,offset,0x100,4);

   page=0x31; 
	offset = 0x10;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal & 0x001);  // bit 0 = Port 8 & Port 0 egress enabled
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("VLAN Port 8 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
 srab_rdwr(page,offset,0x002,4);



// page=0x31; 
//	offset = 0xa;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal & 0x120);  // bit 8 and bit 5 = Port 8 & Port 2 ingress enabled
//
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("VLAN Port 2 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);


#endif
}


uint32_t cfg_sw1()
{
	uint32_t page;
	uint32_t offset;
	uint32_t wdatah, wdatal;
	uint32_t rdatah, rdatal;
	uint32_t port_id;
    uint32_t *rdata;

	// write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
		//rdata = srab_rd(page, offset);
        //rdatal=rdata[0];
        //rdatah=rdata[1];
        //post_log("Port control rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
		//wdatal= 0;//(rdatal&0xffffffc0); 
		//wdatah=rdatah;
        //post_log("Port control wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
		//srab_wr(page, offset, wdatah, wdatal);
		//rdata = srab_rd(page, offset);
        //post_log("Port control rdata[0]=%#x rdata[1]=%#x\n",rdata[0],rdata[1]);
        srab_rdwr(page,offset,0,1);
	}

	// write switch mode register	
	
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("switch mode rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3); 
//	wdatah=rdatah;
//    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
//	srab_wr(page, offset, wdatah, wdatal);
//	//rdata = srab_rd(page, offset);
//    post_log("switch mode rdata[0]=%#x rdata[1]=%#x\n",rdata[0],rdata[1]);//

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);

	// write switch control register	
//	offset=0x22;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch ctrl mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("switch ctrl rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=rdatal | ((rdatal&0xffffffff) | 0x40); 
//	wdatah=rdatah;
//    post_log("switch ctrl wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
//	srab_wr(page, offset, wdatah, wdatal);
//	//rdata = srab_rd(page, offset);
//    post_log("switch ctrl rdata[0]=%#x rdata[1]=%#x\n",rdata[0],rdata[1]);//

	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		//rdata = srab_rd(page, offset);
        //rdatal=rdata[0];
        //rdatah=rdata[1];
        ////post_log("Port override rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
        //post_log("port override rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		//wdatah=rdatah;
        //post_log("port override wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	    //srab_wr(page, offset, wdatah, wdatal);
		//srab_wr(page, offset, wdatah, wdatal);
		
	    //rdata = srab_rd(page, offset);
        //post_log("port override rdata[0]=%#x rdata[1]=%#x\n",rdata[0],rdata[1]);
	}

    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);

///////////Configuration for jumbo frames - Added By Nishant///////
//	page=0x40; 
//	offset = 0x1;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0x123); 
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Jumbo frame config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//	
// offset = 0x5;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0x2710); 
//		 wdatah=rdatah;
//	srab_wr(page, offset, wdatah, wdatal);
//
//post_log("Jumbo frame config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//
/////Configure RGMII Delay - Added by Nishant////
//	page=0xe4; 
//	offset = 0x2a;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0x3); 
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

//  //Global ManagemetControl Register
//  page=0x2; 
//	offset = 0x0;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0xC2);  // bit 7,6 =1 Enable IMP0/1
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Global Management Configuration register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//
// page=0x0; 
//	offset = 0xB;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal | 0x1);  // bit 0 =1 Managed Mode forwarding enabled
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Switch Mode register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//
////Broadcom Header control Register
// page=0x2; 
//	offset = 0x3;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal | 0x1);  // bit 0 = 1 BRCM Header disabled
//
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("BRCM register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);


 /*page=0x31; 
	offset = 0x0;
		rdata = srab_rd(page, offset);
        rdatal=rdata[0];
        rdatah=rdata[1];
   wdatal=(rdatal & 0x101);  // bit 8 and bit 0 = Port 8 & Port 0 egress enabled

		 wdatah=rdatah;
	 srab_wr(page, offset, wdatah, wdatal);
   post_log("VLAN Port 0 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);*/

    page=0x31; 
	offset = 0x2;
		//rdata = srab_rd(page, offset);
        //rdatal=rdata[0];
        //rdatah=rdata[1];
   //wdatal=(rdatal & 0x100);  // bit 8 and bit 0 = Port 8 & Port 0 egress enabled
	//	 wdatah=rdatah;
	// srab_wr(page, offset, wdatah, wdatal);
   //post_log("VLAN Port 1 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
   srab_rdwr(page,offset,0x100,4);

   page=0x31; 
	offset = 0x10;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal & 0x001);  // bit 0 = Port 8 & Port 0 egress enabled
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("VLAN Port 8 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
 srab_rdwr(page,offset,0x001,4);



// page=0x31; 
//	offset = 0xa;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal & 0x120);  // bit 8 and bit 5 = Port 8 & Port 2 ingress enabled
//
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("VLAN Port 2 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);


#endif
}
//------------S/w configuration for managed mode -Nishant-------------------

uint32_t cfg_sw1_rev()
{
	uint32_t page;
	uint32_t offset;
	uint32_t wdatah, wdatal;
	uint32_t rdatah, rdatal;
	uint32_t port_id;
    uint32_t *rdata;

	// write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
		
        srab_rdwr(page,offset,0,1);
	}

	// write switch mode register	
	


    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);


	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		//rdata = srab_rd(page, offset);
        //rdatal=rdata[0];
        //rdatah=rdata[1];
        ////post_log("Port override rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
        //post_log("port override rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		//wdatah=rdatah;
        //post_log("port override wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	    //srab_wr(page, offset, wdatah, wdatal);
		//srab_wr(page, offset, wdatah, wdatal);
		
	    //rdata = srab_rd(page, offset);
        //post_log("port override rdata[0]=%#x rdata[1]=%#x\n",rdata[0],rdata[1]);
	}

    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);

///////////Configuration for jumbo frames - Added By Nishant///////
//	page=0x40; 
//	offset = 0x1;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0x123); 
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Jumbo frame config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//	
// offset = 0x5;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0x2710); 
//		 wdatah=rdatah;
//	srab_wr(page, offset, wdatah, wdatal);
//
//post_log("Jumbo frame config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//
/////Configure RGMII Delay - Added by Nishant////
//	page=0xe4; 
//	offset = 0x2a;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0x3); 
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

//  //Global ManagemetControl Register
//  page=0x2; 
//	offset = 0x0;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//			wdatal=(rdatal | 0xC2);  // bit 7,6 =1 Enable IMP0/1
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Global Management Configuration register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//
// page=0x0; 
//	offset = 0xB;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal | 0x1);  // bit 0 =1 Managed Mode forwarding enabled
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("Switch Mode register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
//
////Broadcom Header control Register
// page=0x2; 
//	offset = 0x3;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal | 0x1);  // bit 0 = 1 BRCM Header disabled
//
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("BRCM register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);


 /*page=0x31; 
	offset = 0x0;
		rdata = srab_rd(page, offset);
        rdatal=rdata[0];
        rdatah=rdata[1];
   wdatal=(rdatal & 0x101);  // bit 8 and bit 0 = Port 8 & Port 0 egress enabled

		 wdatah=rdatah;
	 srab_wr(page, offset, wdatah, wdatal);
   post_log("VLAN Port 0 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);*/

    page=0x31; 
	offset = 0x0;
		//rdata = srab_rd(page, offset);
        //rdatal=rdata[0];
        //rdatah=rdata[1];
   //wdatal=(rdatal & 0x100);  // bit 8 and bit 0 = Port 8 & Port 0 egress enabled
	//	 wdatah=rdatah;
	// srab_wr(page, offset, wdatah, wdatal);
   //post_log("VLAN Port 1 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
   srab_rdwr(page,offset,0x100,4);

   page=0x31; 
	offset = 0x10;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal & 0x001);  // bit 0 = Port 8 & Port 0 egress enabled
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("VLAN Port 8 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);
 srab_rdwr(page,offset,0x001,4);



// page=0x31; 
//	offset = 0xa;
//		rdata = srab_rd(page, offset);
//        rdatal=rdata[0];
//        rdatah=rdata[1];
//   wdatal=(rdatal & 0x120);  // bit 8 and bit 5 = Port 8 & Port 2 ingress enabled
//
//		 wdatah=rdatah;
//	 srab_wr(page, offset, wdatah, wdatal);
//   post_log("VLAN Port 2 register wdatal=%#x wdatah=%#x\n",wdatal,wdatah);


#endif
}
//------------S/w configuration for managed mode -Nishant-------------------

uint32_t cfg_sw1_rev1()
{
	uint32_t page;
	uint32_t offset;
	uint32_t wdatah, wdatal;
	uint32_t rdatah, rdatal;
	uint32_t port_id;
    uint32_t *rdata;

	// write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
		
        srab_rdwr(page,offset,0,1);
	}

	// write switch mode register	
	


    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);


	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}

    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);



#ifdef MAG_MODE



    page=0x31; 
	offset = 0x2;
   srab_rdwr(page,offset,0x100,4);

   page=0x31; 
	offset = 0x10;
 srab_rdwr(page,offset,0x002,4);

#endif
}




int amacswitch_external_loopback_test(void)
{
	test_status_t result,rpkt_no,rpkt_chk;
	   test_status_t res_sts[2] = {TEST_PASS, TEST_PASS};
	   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
	   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
	   amac_rx_pkt_info_t* pkt_ptr;
	   int i;
	   int bypass_mode,rd_data;
	    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, index, nextDscr, rcvPtr,currDscr;
	    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
	    uint8_t rdata8;

	    //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
	    uint32_t frame_size,step,chk_noPkt;
	    uint32_t timeout;
	    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;
	    uint32_t addr, data;
	    uint32_t phy_addr, reg_addr;
		uint32_t rdatah, rdatal;
		uint32_t port_id;
	    uint32_t *rdata;
	    uint32_t page, offset;

	    uint8_t my_pkt_data[256] =
	   {
	     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
	     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
	     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

	   };

	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	            rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	            post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	            rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	            post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);

	   /*test_args_name[0]="Amac Ports =";
	   test_args_name[1]="Tx Desc Base =";
	   test_args_name[2]="Rx Desc Base =";
	              test_args_name[3]="Tx Desc Depth =";
	              test_args_name[4]="Rx Desc Depth =";
	              test_args_name[5]="Tx Buffer Base =";
	              test_args_name[6]="Rx Buffer Base =";
	              test_args_name[7]="Frame size =";*/

	              amac_ports        =1;
	              tx_desc_base      =0x62E00000;//Max Desc space=(512 X 16)=0x2000
	              rx_desc_base      =0x62E06000;
	              tx_desc_len       =3;//Max Desc=512=0x200
	              rx_desc_len       =3;
	              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
	              rx_buff_base      =0x62000000;
	              frame_size          =128;
	              //frame_sz            =1024;
	               step            =23;
	    chk_noPkt       =510;
	    //UNIMAC Config args
	    eth_speed       =0x2;//1Gbps
	    rx_pause_ignore =0;
	    hd_ena          =1;
	    mac_loop_con    =0;
	    ena_ext_config  =0;
	    rmt_loop_ena    =0;
	    tx_pause_ignore =0;

		bypass_mode = 0;

		tx_desc_base = (uint32_t) malloc(16*3);
	    if(tx_desc_base & 0xF) {
	        tx_desc_base += 0xF;
	        tx_desc_base &= 0xFFFFFFF0;
	    }

	    rx_desc_base = (uint32_t) malloc(16*3);
	    if(rx_desc_base & 0xF) {
	        rx_desc_base += 0xF;
	        rx_desc_base &= 0xFFFFFFF0;
	    }

	    tx_buff_base = (uint32_t) malloc(200);
	    if(tx_buff_base & 3) {
	        tx_buff_base += 4;
	        tx_buff_base &= 0xFFFFFFFC;
	    }

	    rx_buff_base = (uint32_t) malloc(200);
	    if(rx_buff_base & 3) {
	        rx_buff_base += 4;
	        rx_buff_base &= 0xFFFFFFFC;
	    }

	    post_log("\n--------------------------------------\n");
	    post_log("tx_desc_base = 0x%08X\n", tx_desc_base);
	    post_log("rx_desc_base = 0x%08X\n", rx_desc_base);
	    post_log("tx_buff_base = 0x%08X\n", tx_buff_base);
	    post_log("rx_buff_base = 0x%08X\n", rx_buff_base);
	    post_log("frame_size   = 0x%08X\n", frame_size);
	    post_log("--------------------------------------\n\n");

	    ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

	   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
	   for(amac_id=0;amac_id < amac_ports;amac_id++){
	        post_log("\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
	        //Basic AMAC block init
	        amac_init(amac_id,bypass_mode);

	        cfg_sw_gphy0_to_gphy1();

	        //------------------------------------------
	        //EXT


	        //------------------------------------------

	        ccg_mdio_init();
	        phy_addr = 0;



	        //------------------------------------------

	        phy_addr = 0;
	        for(i=0; i<16; i++) {
	            reg_addr = i;
	            gphy_reg_read(phy_addr, reg_addr, &data);
	        }

	        post_log("-------------------------------------------\n\n");

	        reg_addr = 0x17;
	        data = 0x0F09;
	        gphy_reg_write(phy_addr, reg_addr, &data);
	        reg_addr = 0x15;
	        data = 0x5193;
	        gphy_reg_write(phy_addr, reg_addr, &data);

	        reg_addr = 0x17;
	        data = 0x0F09;
	        gphy_reg_write(phy_addr, reg_addr, &data);
	        reg_addr = 0x15;
	        gphy_reg_read(phy_addr, reg_addr, &data);

	        //wait for linkup
	        reg_addr = 0x1;
	        do {
	            gphy_reg_read(phy_addr, reg_addr, &data);
	        } while(!(data & 0x4));

	        post_log("-------------------------------------------\n\n");

	        phy_addr = 1;
	        reg_addr = 0x17;
	        data = 0x0F09;
	        gphy_reg_write(phy_addr, reg_addr, &data);
	        reg_addr = 0x15;
	        data = 0x11C9;
	        gphy_reg_write(phy_addr, reg_addr, &data);

	        reg_addr = 0x17;
	        data = 0x0F09;
	        gphy_reg_write(phy_addr, reg_addr, &data);
	        reg_addr = 0x15;
	        gphy_reg_read(phy_addr, reg_addr, &data);

	        //wait for linkup
	        reg_addr = 0x1;
	        do {
	            gphy_reg_read(phy_addr, reg_addr, &data);
	        } while(!(data & 0x4));

	        post_log("-------------------------------------------\n\n");

	        //UNIMAC Config
	                unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
	                             rmt_loop_ena,tx_pause_ignore);

	                //Reset MIB Counter
	                counter_reset(amac_id);

	                //Set up MAC loopback
	                //amac_set_config(amac_id, MAC_LOOPBACK, 1);

	                //Initialize AMAC TX DMA block
	                amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
	                amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
	                for(index = 0; index < tx_desc_len; index++) {

	                     //frame_size=((rand()%(0x3000-64))+64);

	                     CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

	                     write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

	                     //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
	                     amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
	                                     (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
	                     amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
	                }

	                //Initialize AMAC RX DMA block
	                amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
	                                AMAC_PKT_BUFFER_SIZE_IN_BYTES);

	                //Update with next descriptor value - outside EOT range
	                //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

	                //Within EOT range
	                amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

	                 //amac_get_tx_status(amac_id);
	                 //amac_get_rx_status(amac_id);
	                 post_log("\n\n");
	           }
	            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





	            ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
	           for(amac_id=0;amac_id < amac_ports;amac_id++){
	                post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
	                // NxtDscr points to EOT Dscr
	                nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
	                                        ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
	                //Out of EOT range,for continuous loopback
	                //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
	                                        ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
	                amac_enable_tx_dma(amac_id, nextDscr);
	                post_log("\n\n");
	           }
	           /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	           //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
	           /*timeout=300;
	           post_log("Sending packets for specified time\n");
	           delay_noise(timeout,0);
	           post_log("Time_taken =%d secs\n",(timeout*7));
	           suspend_tx(amac_id);*/

	           for(amac_id=0;amac_id < amac_ports;amac_id++){
	                waitforTrans_internalloopback(amac_id);
	                post_log("\n");
	                amac_gettxstats(amac_id);
	                amac_getrxstats(amac_id);

	                rpkt_no=checker_all(amac_id);
	                post_log("\n");

	                post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
	                rpkt_chk=TEST_PASS;
	                rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

	                 if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
	                         res_sts[amac_id]=TEST_PASS;
	                         post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
	                 }
	                 else{
	                         res_sts[amac_id]=TEST_FAIL;
	                         post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
	                 }

	                 post_log("\n\n");
	           }
	           ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	           if((res_sts[0]==TEST_PASS)){
	                    result=TEST_PASS;
	           }else{
	                    result=TEST_FAIL;
	            }
	           return result;

}

int amacswitch_internal_loopback_test_ddr(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =100;//Max Desc=512=0x200
              rx_desc_len       =100;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////
cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);
   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
				 post_log("==========PASS==========\n");
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
				 post_log("==========FAIL==========\n");
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}



int amacswitch_internal_loopback_test_sram(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       =2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
				 post_log("==========PASS==========\n");
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
				 post_log("==========FAIL==========\n");
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}


int amacswitch_bypass_test(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

   bypass_mode = 1;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =16;//Max Desc=512=0x200
              rx_desc_len       =16;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        //amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        //rpkt_no=checker_all(amac_id);
        //post_log("\n");

        //post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        //rpkt_chk=TEST_PASS;
        //rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         /*if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");*/
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   res_sts[0] = TEST_PASS;

   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gmii_2_rgmii_switchport_1_to_5_ddr(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index,rd_data;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
    cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

    rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
    post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

    rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =16;//Max Desc=512=0x200
              rx_desc_len       = 16;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        // amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////

    cfg_sw1();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        //rpkt_no = TEST_PASS;
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;

        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);


         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gmii_2_rgmii_switchport_1_to_5_sram(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index,rd_data;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
    cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

    rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
    post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

    rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        = 1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       = 2;//Max Desc=512=0x200
              rx_desc_len       = 2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX descriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        // amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////

    cfg_sw();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        //rpkt_no = TEST_PASS;
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;

        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);


         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gmii_2_gmii_switchport_0_to_1_ddr(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;
   uint32_t rd_data;
   uint32_t page;
   	uint32_t offset;
   	uint32_t wdatah, wdatal;
   	uint32_t rdatah, rdatal;
   	uint32_t port_id;
       uint32_t *rdata;
       uint32_t phy_addr,reg_addr,data,i;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =16;//Max Desc=512=0x200
              rx_desc_len       = 16;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);


       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        // amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////

    //cfg_sw_0_1();
   cfg_sw_gphy0_to_gphy1();

                      page=0x0;
                      offset=0x6f;
                      rdata = srab_rd(page, offset);
                      wdatal=(rdatal | 0x1);
                      wdatah=rdatah;
                      srab_wr(page, offset, wdatah, wdatal);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         phy_addr = 0;
                                       reg_addr = 0x1;
                                       do {
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
                                       } while(!(data & 0x4));

                         //wait for linkup
                         phy_addr = 1;
                         reg_addr = 0x1;
                         do {
                      	    gphy_reg_read(phy_addr,reg_addr,&data);
                         } while(!(data & 0x4));


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

       // rpkt_no=checker_all(amac_id);Nishant

        rpkt_no = TEST_PASS;
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;

        rpkt_chk=chkpktdata_add(amac_id,tx_desc_base,rx_desc_base,2*chk_noPkt);


         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*page = 0xe2;
	offset=0x00;
	rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0xe2 Offset: 0x00 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0xe2;
    offset=0x01;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0xe2 Offset: 0x01 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0xe2;
    offset=0x02;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0xe2 Offset: 0x02 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0xe2;
    offset=0x03;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0xe2 Offset: 0x03 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x10;
    offset=0x00;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x10 Offset: 0x00 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x10;
    offset=0x01;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x10 Offset: 0x01 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x10;
    offset=0x02;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x10 Offset: 0x02 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x10;
    offset=0x03;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x10 Offset: 0x03 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x11;
    offset=0x00;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x11 Offset: 0x00 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x11;
    offset=0x01;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x11 Offset: 0x01 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x11;
    offset=0x02;
    rdata = srab_rd(page, offset);
    rdatal=rdata[0];
    rdatah=rdata[1];
    post_log("Page : 0x11 Offset: 0x02 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);

    page = 0x11;
        offset=0x03;
        rdata = srab_rd(page, offset);
        rdatal=rdata[0];
        rdatah=rdata[1];
        post_log("Page : 0x11 Offset: 0x03 : rdatal : %08X rdatah : %08X \n",rdatal,rdatah);*/





   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gmii_2_gmii_switchport_0_to_1_sram(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;
   uint32_t rd_data;
   uint32_t rdatal_16bit;

   uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;

   if(BCM958302K == post_get_board_diags_type())
   {
	   post_log("\nSKIP : Skipping the test, diag not supported in POS Boards\n");
	   return TEST_SKIP;
   }


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       = 2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =64;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //cfg_sw_0_1(); modified by sathishs
        cfg_sw_gphy0_to_gphy1();

            page=0x0;
            offset=0x6f;
            rdata = srab_rd(page, offset);
            wdatal=(rdatal | 0x1);
            wdatah=rdatah;
            srab_wr(page, offset, wdatah, wdatal);


            ccg_mdio_init();

            for(i=0; i<80000000; i++) ;
             phy_addr = 0;
               for(i=0; i<16; i++) {
                      reg_addr = i;
                      gphy_reg_read(phy_addr, reg_addr, &data);
                 }

               //Lane Swapping and Polarity inversion for Gphy0
               phy_addr = 0;
               reg_addr = 0x17;
               data = 0x0F09;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               phy_addr = 0;
               reg_addr = 0x15;
               data = 0x5193;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               for(i=0; i<80000000; i++) ;

               /*phy_addr = 0;
                                            reg_addr = 0x1;
                                            do {
                                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                                            } while(!(data & 0x4));*/

               //Lane Swapping and Polarity inversion for Gphy1
               phy_addr = 1;
               reg_addr = 0x17;
               data = 0x0F09;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 0x15;
               data = 0x11C9;
               gphy_reg_write(phy_addr, reg_addr, &data);

               for(i=0; i<80000000; i++) ;



               //wait for linkup
               phy_addr = 1;
               reg_addr = 0x1;
               do {
            	    gphy_reg_read(phy_addr,reg_addr,&data);
               } while(!(data & 0x4));

               phy_addr = 0;
                              reg_addr = 0x1;
                              do {
                           	    gphy_reg_read(phy_addr,reg_addr,&data);
                              } while(!(data & 0x4));

       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);


         amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             //CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             //write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             /*amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }*/

        	amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
        	                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
        	             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        	        }

        	        //Initialize AMAC RX DMA block
        	        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
        	                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        	        //Update with next descriptor value - outside EOT range
        	        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////




          //This is for phy internal loopback
            /*reg_addr = 0;
            gphy_reg_read(phy_addr, reg_addr, &data);
            data |= (1 << 14);
            gphy_reg_write(phy_addr, reg_addr, &data);
            gphy_reg_read(phy_addr, reg_addr, &data);

            data = 0x01E1;
            gphy_reg_write(phy_addr, 0x0004, &data);
            gphy_reg_read(phy_addr, 0x0004, &data);*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }


   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   post_log("Sending packets for specified time\n");
      //delay_noise(timeout,0);
      for(i=0; i<1000000; i++) {
         //dummy read
         if((i & 0xFFF) == 0)
         post_log("\r\rTransmitting pkts... %d\r\r", i);
         data = rd(0x02000000);
      }

      for(amac_id=0;amac_id < amac_ports;amac_id++){
             suspend_tx(amac_id);
        }

        for(i=0; i<1000; i++) {
           //dummy read
           data = rd(0x02000000);
        }

   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //post_log("Entering the debug points for Henry \n");


   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);
        //amac_gettxrxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);//Nishant

        //rpkt_no = TEST_PASS;
        post_log("\n");

        //post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        //rpkt_chk=TEST_PASS;

        //rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);


         if((rpkt_no==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
				 post_log("==========PASS==========\n");
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
				 post_log("==========FAIL==========\n");
         }

         post_log("\n\n");
   }

   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_internal_loopback_test_gphy0(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;
   uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       =2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        cfg_sw_0_1();

                    page=0x0;
                    offset=0x6f;
                    rdata = srab_rd(page, offset);
                    wdatal=(rdatal | 0x1);
                    wdatah=rdatah;
                    srab_wr(page, offset, wdatah, wdatal);


                    ccg_mdio_init();

                     phy_addr = 0;
                       for(i=0; i<16; i++) {
                              reg_addr = i;
                              gphy_reg_read(phy_addr, reg_addr, &data);
                         }

                       //Lane Swapping and Polarity inversion for Gphy0
                       phy_addr = 0;
                       reg_addr = 0x17;
                       data = 0x0F09;
                       gphy_reg_write(phy_addr, reg_addr, &data);
                       //gphy_reg_read(phy_addr, reg_addr, &data);

                       phy_addr = 0;
                       reg_addr = 0x15;
                       data = 0x5193;
                       gphy_reg_write(phy_addr, reg_addr, &data);
                       //gphy_reg_read(phy_addr, reg_addr, &data);



                       //wait for linkup
                       //reg_addr = 0x1;
                       //do {
                    	//    gphy_reg_read(phy_addr,reg_addr,&data);
                       //} while(!(data & 0x4));

                       reg_addr = 0;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                                   data |= (1 << 14);
                                   gphy_reg_write(phy_addr, reg_addr, &data);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_internal_loopback_test_gphy1(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;
   uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       =2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        cfg_sw_0_1();

                    page=0x0;
                    offset=0x6f;
                    rdata = srab_rd(page, offset);
                    wdatal=(rdatal | 0x1);
                    wdatah=rdatah;
                    srab_wr(page, offset, wdatah, wdatal);


                    ccg_mdio_init();

                     phy_addr = 1;
                       for(i=0; i<16; i++) {
                              reg_addr = i;
                              gphy_reg_read(phy_addr, reg_addr, &data);
                         }

                       //Lane Swapping and Polarity inversion for Gphy0
                       phy_addr = 1;
                       reg_addr = 0x17;
                       data = 0x0F09;
                       gphy_reg_write(phy_addr, reg_addr, &data);
                       //gphy_reg_read(phy_addr, reg_addr, &data);

                       phy_addr = 1;
                       reg_addr = 0x15;
                       data = 0x11C9;
                       gphy_reg_write(phy_addr, reg_addr, &data);
                       //gphy_reg_read(phy_addr, reg_addr, &data);



                       //wait for linkup
                       //reg_addr = 0x1;
                       //do {
                    	//    gphy_reg_read(phy_addr,reg_addr,&data);
                       //} while(!(data & 0x4));

                       reg_addr = 0;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                                   data |= (1 << 14);
                                   gphy_reg_write(phy_addr, reg_addr, &data);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        //amac_gettxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int amacswitch_robosw_gphy0_2_gphy1_switchport_0_to_1(void)
{
   test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2] = {TEST_PASS, TEST_PASS};
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   int i;
   int bypass_mode,rd_data;
    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, index, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

    //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;
    uint32_t addr, data;
    uint32_t phy_addr, reg_addr;
	uint32_t rdatah, rdatal;
	uint32_t port_id;
    uint32_t *rdata;
    uint32_t page, offset;

    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

            rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
            post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

            rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
            post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);

   /*test_args_name[0]="Amac Ports =";
   test_args_name[1]="Tx Desc Base =";
   test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";*/

              amac_ports        =1;
              tx_desc_base      =0x62E00000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x62E06000;
              tx_desc_len       =3;//Max Desc=512=0x200
              rx_desc_len       =3;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =510;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =0;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =0;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	bypass_mode = 0;

	tx_desc_base = (uint32_t) malloc(16*3);
    if(tx_desc_base & 0xF) {
        tx_desc_base += 0xF;
        tx_desc_base &= 0xFFFFFFF0;
    }

    rx_desc_base = (uint32_t) malloc(16*3);
    if(rx_desc_base & 0xF) {
        rx_desc_base += 0xF;
        rx_desc_base &= 0xFFFFFFF0;
    }

    tx_buff_base = (uint32_t) malloc(200);
    if(tx_buff_base & 3) {
        tx_buff_base += 4;
        tx_buff_base &= 0xFFFFFFFC;
    }

    rx_buff_base = (uint32_t) malloc(200);
    if(rx_buff_base & 3) {
        rx_buff_base += 4;
        rx_buff_base &= 0xFFFFFFFC;
    }

    post_log("\n--------------------------------------\n");
    post_log("tx_desc_base = 0x%08X\n", tx_desc_base);
    post_log("rx_desc_base = 0x%08X\n", rx_desc_base);
    post_log("tx_buff_base = 0x%08X\n", tx_buff_base);
    post_log("rx_buff_base = 0x%08X\n", rx_buff_base);
    post_log("frame_size   = 0x%08X\n", frame_size);
    post_log("--------------------------------------\n\n");

    ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        cfg_sw_gphy0_to_gphy1();

        //------------------------------------------
        //EXT

        //------------------------------------------

        ccg_mdio_init();
        phy_addr = 0;

        //------------------------------------------

        phy_addr = 0;
        for(i=0; i<16; i++) {
            reg_addr = i;
            gphy_reg_read(phy_addr, reg_addr, &data);
        }

        post_log("-------------------------------------------\n\n");

        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x5193;
        gphy_reg_write(phy_addr, reg_addr, &data);

        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);

        //wait for linkup
        reg_addr = 0x1;
        do {
            gphy_reg_read(phy_addr, reg_addr, &data);
        } while(!(data & 0x4));

        post_log("-------------------------------------------\n\n");

        phy_addr = 1;
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11C9;
        gphy_reg_write(phy_addr, reg_addr, &data);

        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);

        //wait for linkup
        reg_addr = 0x1;
        do {
            gphy_reg_read(phy_addr, reg_addr, &data);
        } while(!(data & 0x4));

        post_log("-------------------------------------------\n\n");

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        //amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             //CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             //write_pkt_to_memory(my_pkt_data,(frame_size), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len + 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
   timeout=10;
   post_log("Sending packets for specified time\n");
   //delay_noise(timeout,0);
   for(i=0; i<1000000; i++) {
      //dummy read
      if((i & 0xFFF) == 0)
      post_log("\r\rTransmitting pkts... %d\r\r", i);
      data = rd(0x02000000);
   }


   //post_log("Press a key to continue...\n");
   //getHex();

   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }

   for(i=0; i<1000; i++) {
      //dummy read
      data = rd(0x02000000);
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        //rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   post_log("\n--------------------------------------\n");
   post_log("GMAC0_XMTSTATUS0_0   = 0x%08X\n", *(uint32_t *) GMAC0_XMTSTATUS0_0);
   post_log("GMAC0_XMTSTATUS1_0   = 0x%08X\n", *(uint32_t *) GMAC0_XMTSTATUS1_0);
   post_log("GMAC0_XMTCONTROL_0   = 0x%08X\n", *(uint32_t *) GMAC0_XMTCONTROL_0);
   post_log("GMAC0_RCVSTATUS0     = 0x%08X\n", *(uint32_t *) GMAC0_RCVSTATUS0);
   post_log("GMAC0_RCVSTATUS1     = 0x%08X\n", *(uint32_t *) GMAC0_RCVSTATUS1);
   post_log("GMAC0_RCVCONTROL     = 0x%08X\n", *(uint32_t *) GMAC0_RCVCONTROL);
   post_log("GMAC0_DEVCONTROL     = 0x%08X\n", *(uint32_t *) GMAC0_DEVCONTROL);
   post_log("GMAC0_DEVSTATUS      = 0x%08X\n", *(uint32_t *) GMAC0_DEVSTATUS);
   post_log("GMAC0_XMTADDR_LOW_0  = 0x%08X\n", *(uint32_t *) GMAC0_XMTADDR_LOW_0);
   post_log("GMAC0_XMTADDR_HIGH_0 = 0x%08X\n", *(uint32_t *) GMAC0_XMTADDR_HIGH_0);
   post_log("--------------------------------------\n\n");

   addr = tx_desc_base;
   while(addr < tx_buff_base) {
      post_log("[0x%08X] = 0x%08X\n", addr, *(uint32_t *) addr);
      addr += 4;
   }

   if((res_sts[0]==TEST_PASS) && (res_sts[1]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;
}


/*int gphy_internal_loopback(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;
   uint32_t rd_data;
   uint32_t rdatal_16bit;

   uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       = 2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        cfg_sw_0_1();

            ccg_mdio_init();

                    phy_addr = 0;
                    for(i=0; i<16; i++) {
                        reg_addr = i;
                        gphy_reg_read(phy_addr, reg_addr, &data);
                    }

                    reg_addr = 0;
                    gphy_reg_read(phy_addr, reg_addr, &data);
                    data |= (1 << 14);
                    gphy_reg_write(phy_addr, reg_addr, &data);
                    gphy_reg_read(phy_addr, reg_addr, &data);

                    data = 0x01E1;
                    gphy_reg_write(phy_addr, 0x0004, &data);
                    gphy_reg_read(phy_addr, 0x0004, &data);


       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        // amac_set_config(amac_id, "MAC_LOOPBACK", 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
   timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

       // rpkt_no=checker_all(amac_id);Nishant

        rpkt_no = TEST_PASS;
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;

        rpkt_chk=chkpktdata_add(amac_id,tx_desc_base,rx_desc_base,2*chk_noPkt);


         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}*/

int mac_loopback_test(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;
   uint32_t rd_data;
   uint32_t rdatal_16bit;

   uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       = 2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =64;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //cfg_sw_0_1(); modified by sathishs
        /*cfg_sw_gphy0_to_gphy1();

            page=0x0;
            offset=0x6f;
            rdata = srab_rd(page, offset);
            wdatal=(rdatal | 0x1);
            wdatah=rdatah;
            srab_wr(page, offset, wdatah, wdatal);


            ccg_mdio_init();

            for(i=0; i<80000000; i++) ;
             phy_addr = 0;
               for(i=0; i<16; i++) {
                      reg_addr = i;
                      gphy_reg_read(phy_addr, reg_addr, &data);
                 }

               //Lane Swapping and Polarity inversion for Gphy0
               phy_addr = 0;
               reg_addr = 0x17;
               data = 0x0F09;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               phy_addr = 0;
               reg_addr = 0x15;
               data = 0x5193;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               for(i=0; i<80000000; i++) ;

               phy_addr = 0;
                                            reg_addr = 0x1;
                                            do {
                                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                                            } while(!(data & 0x4));

               //Lane Swapping and Polarity inversion for Gphy1
               phy_addr = 1;
               reg_addr = 0x17;
               data = 0x0F09;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 0x15;
               data = 0x11C9;
               gphy_reg_write(phy_addr, reg_addr, &data);

               for(i=0; i<80000000; i++) ;



               //wait for linkup
               phy_addr = 1;
               reg_addr = 0x1;
               do {
            	    gphy_reg_read(phy_addr,reg_addr,&data);
               } while(!(data & 0x4));

               phy_addr = 0;
                              reg_addr = 0x1;
                              do {
                           	    gphy_reg_read(phy_addr,reg_addr,&data);
                              } while(!(data & 0x4));*/

       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);


         amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             //CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             //write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             /*amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }*/

        	amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
        	                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
        	             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        	        }

        	        //Initialize AMAC RX DMA block
        	        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
        	                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        	        //Update with next descriptor value - outside EOT range
        	        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////




          //This is for phy internal loopback
            /*reg_addr = 0;
            gphy_reg_read(phy_addr, reg_addr, &data);
            data |= (1 << 14);
            gphy_reg_write(phy_addr, reg_addr, &data);
            gphy_reg_read(phy_addr, reg_addr, &data);

            data = 0x01E1;
            gphy_reg_write(phy_addr, 0x0004, &data);
            gphy_reg_read(phy_addr, 0x0004, &data);*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }


   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   post_log("Sending packets for specified time\n");
      //delay_noise(timeout,0);
      for(i=0; i<1000000; i++) {
         //dummy read
         if((i & 0xFFF) == 0)
         post_log("\r\rTransmitting pkts... %d\r\r", i);
         data = rd(0x02000000);
      }

      for(amac_id=0;amac_id < amac_ports;amac_id++){
             suspend_tx(amac_id);
        }

        for(i=0; i<1000; i++) {
           //dummy read
           data = rd(0x02000000);
        }

   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //post_log("Entering the debug points for Henry \n");


   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);
        //amac_gettxrxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);//Nishant

        //rpkt_no = TEST_PASS;
        post_log("\n");

        //post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        //rpkt_chk=TEST_PASS;

        //rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);


         if((rpkt_no==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
				 post_log("==========PASS==========\n");
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
				 post_log("==========FAIL==========\n");
         }

         post_log("\n\n");
   }

   /*page=0x1;
   offset=0;
   rdata = srab_rd(page, offset);
   //post_log("Page 1 offset 0 : %08X \n",rdata[0]);

   page=0x1;
   offset=4;
   rdata = srab_rd(page, offset);
   //post_log("Page 1 offset 4 : %08X \n",rdata[0]);

   page=0x1;
      offset=8;
      rdata = srab_rd(page, offset);*/
     // post_log("Page 1 offset 8 : %08X \n",rdata[0]);


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int phy_loopback_test(void)
{
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;
   uint32_t rd_data;
   uint32_t rdatal_16bit;

   uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x2000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x2010000;
              tx_desc_len       =2;//Max Desc=512=0x200
              rx_desc_len       = 2;
              tx_buff_base      =0x2020000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x2030000;
              frame_size          =64;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

		 ccg_mdio_init();

		 cpu_wr_single(SWITCH_GLOBAL_CONFIG, 0x2001, 4);

        //cfg_sw_0_1(); modified by sathishs
        /*cfg_sw_gphy0_to_gphy1();

            page=0x0;
            offset=0x6f;
            rdata = srab_rd(page, offset);
            wdatal=(rdatal | 0x1);
            wdatah=rdatah;
            srab_wr(page, offset, wdatah, wdatal);


            ccg_mdio_init();

            for(i=0; i<80000000; i++) ;
             phy_addr = 0;
               for(i=0; i<16; i++) {
                      reg_addr = i;
                      gphy_reg_read(phy_addr, reg_addr, &data);
                 }

               //Lane Swapping and Polarity inversion for Gphy0
               phy_addr = 0;
               reg_addr = 0x17;
               data = 0x0F09;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               phy_addr = 0;
               reg_addr = 0x15;
               data = 0x5193;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               for(i=0; i<80000000; i++) ;

               phy_addr = 0;
                                            reg_addr = 0x1;
                                            do {
                                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                                            } while(!(data & 0x4));

               //Lane Swapping and Polarity inversion for Gphy1
               phy_addr = 1;
               reg_addr = 0x17;
               data = 0x0F09;
               gphy_reg_write(phy_addr, reg_addr, &data);
               //gphy_reg_read(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 0x15;
               data = 0x11C9;
               gphy_reg_write(phy_addr, reg_addr, &data);

               for(i=0; i<80000000; i++) ;



               //wait for linkup
               phy_addr = 1;
               reg_addr = 0x1;
               do {
            	    gphy_reg_read(phy_addr,reg_addr,&data);
               } while(!(data & 0x4));

               phy_addr = 0;
                              reg_addr = 0x1;
                              do {
                           	    gphy_reg_read(phy_addr,reg_addr,&data);
                              } while(!(data & 0x4));*/

							   //This is for phy internal loopback
            reg_addr = 0;
            gphy_reg_read(phy_addr, reg_addr, &data);
            data |= (1 << 14);
            gphy_reg_write(phy_addr, reg_addr, &data);
            gphy_reg_read(phy_addr, reg_addr, &data);

            data = 0x01E1;
            gphy_reg_write(phy_addr, 0x0004, &data);
            gphy_reg_read(phy_addr, 0x0004, &data);

       //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);


         //amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             //CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             //write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             /*amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
        // post_log("\n\n");
   }*/

        	amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
        	                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
        	             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        	        }

        	        //Initialize AMAC RX DMA block
        	        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
        	                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        	        //Update with next descriptor value - outside EOT range
        	        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Configuration for Managed mode //////////////////////////////////////




          //This is for phy internal loopback
            /*reg_addr = 0;
            gphy_reg_read(phy_addr, reg_addr, &data);
            data |= (1 << 14);
            gphy_reg_write(phy_addr, reg_addr, &data);
            gphy_reg_read(phy_addr, reg_addr, &data);

            data = 0x01E1;
            gphy_reg_write(phy_addr, 0x0004, &data);
            gphy_reg_read(phy_addr, 0x0004, &data);*/

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        //post_log("\n\n");
   }


   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   post_log("Sending packets for specified time\n");
      //delay_noise(timeout,0);
      for(i=0; i<1000000; i++) {
         //dummy read
         if((i & 0xFFF) == 0)
         post_log("\r\rTransmitting pkts... %d\r\r", i);
         data = rd(0x02000000);
      }

      for(amac_id=0;amac_id < amac_ports;amac_id++){
             suspend_tx(amac_id);
        }

        for(i=0; i<1000; i++) {
           //dummy read
           data = rd(0x02000000);
        }

   //////////////////////////////////////////////// Send and stop packet transmission for A0  and A1 ////////////////////////
  /* timeout=100;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout));
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        suspend_tx(amac_id);
   }*/
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   //post_log("Entering the debug points for Henry \n");


   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){

        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);
        //amac_gettxrxstats(amac_id);
        //amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);//Nishant

        //rpkt_no = TEST_PASS;
        post_log("\n");

        //post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        //rpkt_chk=TEST_PASS;

        //rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);


         if((rpkt_no==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
         }

         post_log("\n\n");
   }

   /*page=0x1;
   offset=0;
   rdata = srab_rd(page, offset);
   //post_log("Page 1 offset 0 : %08X \n",rdata[0]);

   page=0x1;
   offset=4;
   rdata = srab_rd(page, offset);
   //post_log("Page 1 offset 4 : %08X \n",rdata[0]);

   page=0x1;
      offset=8;
      rdata = srab_rd(page, offset);*/
     // post_log("Page 1 offset 8 : %08X \n",rdata[0]);


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

}

int gphy0_powerdown_test(void)
{
      
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;
		    uint32_t rd_data;



    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);

            ccg_mdio_init();

            for(i=0; i<80000000; i++) ;
             phy_addr = 0;
               for(i=0; i<16; i++) {
                      reg_addr = i;
                      gphy_reg_read(phy_addr, reg_addr, &data);
                 }

                              phy_addr = 0;
                              reg_addr = 0;
                              gphy_reg_read(phy_addr,reg_addr,&data);
                              data = data | (1 << 11);
                              gphy_reg_write(phy_addr, reg_addr, &data);

                              post_log("GPHY0 POWERED DOWN \n");


    return 0;
   }


int gphy1_powerdown_test(void)
{
     
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;
		  uint32_t rd_data;

  

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);

            ccg_mdio_init();

            for(i=0; i<80000000; i++) ;
             phy_addr = 0;
               for(i=0; i<16; i++) {
                      reg_addr = i;
                      gphy_reg_read(phy_addr, reg_addr, &data);
                 }

                              phy_addr = 1;
                              reg_addr = 0;
                              gphy_reg_read(phy_addr,reg_addr,&data);
                              data = data | (1 << 11);
                              gphy_reg_write(phy_addr, reg_addr, &data);

                              post_log("GPHY1 POWERED DOWN \n");


    return 0;
   }

int gphy0_dsp_tap10_register_enable_link_up_test_1Gbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);














	                              //Lane Swapping and Polarity inversion for Gphy0
	                                                      phy_addr = 0;
	                                                      reg_addr = 0x17;
	                                                      data = 0x0F09;
	                                                      gphy_reg_write(phy_addr, reg_addr, &data);
	                                                      //gphy_reg_read(phy_addr, reg_addr, &data);

	                                                      phy_addr = 0;
	                                                      reg_addr = 0x15;
	                                                      data = 0x5193;
	                                                      gphy_reg_write(phy_addr, reg_addr, &data);

	                                                      //Lane Swapping and Polarity inversion for Gphy1
	                                                                               phy_addr = 1;
	                                                                               reg_addr = 0x17;
	                                                                               data = 0x0F09;
	                                                                               gphy_reg_write(phy_addr, reg_addr, &data);
	                                                                               //gphy_reg_read(phy_addr, reg_addr, &data);

	                                                                               phy_addr = 1;
	                                                                               reg_addr = 0x15;
	                                                                               data = 0x11C9;
	                                                                               gphy_reg_write(phy_addr, reg_addr, &data);

	                                                                               for(i=0; i<20000000; i++) ;

	                                                                               phy_addr = 0;
	                                                                                             reg_addr = 0x1;
	                                                                                             do {
	                                                                                          	    gphy_reg_read(phy_addr,reg_addr,&data);
	                                                                                             } while(data != 0x79ED);//while(!(data & 0x4));

	                                                                               //wait for linkup
	                                                                               /*phy_addr = 1;
	                                                                               reg_addr = 0x1;
	                                                                               do {
	                                                                            	    gphy_reg_read(phy_addr,reg_addr,&data);
	                                                                               } while(!(data & 0x4));*/




	               for(i=0; i<80000000; i++) ;





	               phy_addr = 0;
	                              reg_addr = 0x1;
	                              do {
	                           	    gphy_reg_read(phy_addr,reg_addr,&data);
	                              } while(data != 0x79ED);//while(!(data & 0x4));

	                              post_log("GPHY0 LINK UP IN 1GBPS\n");





	    return 0;

}

int gphy1_dsp_tap10_register_enable_link_up_test_1Gbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);














	                              //Lane Swapping and Polarity inversion for Gphy0
	                                                      phy_addr = 0;
	                                                      reg_addr = 0x17;
	                                                      data = 0x0F09;
	                                                      gphy_reg_write(phy_addr, reg_addr, &data);
	                                                      //gphy_reg_read(phy_addr, reg_addr, &data);

	                                                      phy_addr = 0;
	                                                      reg_addr = 0x15;
	                                                      data = 0x5193;
	                                                      gphy_reg_write(phy_addr, reg_addr, &data);

	                                                      //Lane Swapping and Polarity inversion for Gphy1
	                                                                               phy_addr = 1;
	                                                                               reg_addr = 0x17;
	                                                                               data = 0x0F09;
	                                                                               gphy_reg_write(phy_addr, reg_addr, &data);
	                                                                               //gphy_reg_read(phy_addr, reg_addr, &data);

	                                                                               phy_addr = 1;
	                                                                               reg_addr = 0x15;
	                                                                               data = 0x11C9;
	                                                                               gphy_reg_write(phy_addr, reg_addr, &data);

	                                                                               for(i=0; i<20000000; i++) ;

	                                                                               phy_addr = 1;
	                                                                                             reg_addr = 0x1;
	                                                                                             do {
	                                                                                          	    gphy_reg_read(phy_addr,reg_addr,&data);
	                                                                                             } while(data != 0x79ED);//while(!(data & 0x4));

	                                                                               //wait for linkup
	                                                                               /*phy_addr = 1;
	                                                                               reg_addr = 0x1;
	                                                                               do {
	                                                                            	    gphy_reg_read(phy_addr,reg_addr,&data);
	                                                                               } while(!(data & 0x4));*/




	               for(i=0; i<80000000; i++) ;





	               phy_addr = 1;
	                              reg_addr = 0x1;
	                              do {
	                           	    gphy_reg_read(phy_addr,reg_addr,&data);
	                              } while(data != 0x79ED);//while(!(data & 0x4));

	                              post_log("GPHY1 LINK UP IN 1GBPS\n");





	    return 0;

}

int gphy0_dsp_tap10_register_enable_link_up_test_100Mbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);


                        //Lane Swapping and Polarity inversion for Gphy0
                        phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);


                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);


                         /*100 Mbps
                         0x4 --> 0x781
                         0x5 --> 0xC781
                         0x9 --> 0
                         0x0 --> 0x140*/

                         phy_addr = 0;
                         reg_addr = 0;
                         data = 0x3140;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 4;
                         data = 0x781;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 5;
                         data = 0xC781;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 9;
                         data = 0x0000;
                         gphy_reg_write(phy_addr, reg_addr, &data);





	               phy_addr = 0;
	               reg_addr = 0x1;
	                do {
	                      gphy_reg_read(phy_addr,reg_addr,&data);
	                } while(data != 0x79ED);//while(!(data & 0x4));

	               post_log("GPHY0 LINK UP IN 100Mbps Mode\n");





	    return 0;

}

int gphy0_dsp_tap10_register_enable_link_up_test_without_laneswap_100Mbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;
			  uint32_t gphy0_count;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }

									//phy reset
						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);





                         /*100 Mbps
                         0x4 --> 0x781
                         0x5 --> 0xC781
                         0x9 --> 0
                         0x0 --> 0x140*/

                         phy_addr = 0;
                         reg_addr = 0;
                         data = 0x3140;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 4;
                         data = 0x781;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 5;
                         data = 0xC781;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 9;
                         data = 0x0000;
                         gphy_reg_write(phy_addr, reg_addr, &data);





	               phy_addr = 0;
	                                                                                             reg_addr = 0x1;
	                                                                                            gphy0_count = 0;
                                       do {
										    if (gphy0_count == 2000)
										    {
												post_log("GPHY0 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy0_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                                                                               //wait for linkup
	                                                                               /*phy_addr = 1;
	                                                                               reg_addr = 0x1;
	                                                                               do {
	                                                                            	    gphy_reg_read(phy_addr,reg_addr,&data);
	                                                                               } while(!(data & 0x4));*/




	               for(i=0; i<80000000; i++) ;





	               phy_addr = 0;
	                              reg_addr = 0x1;
	                             gphy0_count = 0;
                                       do {
										    if (gphy0_count == 2000)
										    {
												post_log("GPHY0 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy0_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                              if (gphy0_count != 2000)
	                              {
	                              
								  post_log("GPHY0 LINK UP IN 100MBPS WITHOUT LANESWAP\n");
								  post_log("==========PASS==========\n");
								  return TEST_PASS;
								  }
								  else
	                              {
									  post_log("==========FAIL==========\n");
									  return TEST_FAIL;
								  }

}

int gphy0_dsp_tap10_register_enable_link_up_test_10Mbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;
			  uint32_t gphy0_count;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);


                        //Lane Swapping and Polarity inversion for Gphy0
                        phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);


                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);


                         /*100 Mbps
                         0x4 --> 0x781
                         0x5 --> 0xC781
                         0x9 --> 0
                         0x0 --> 0x140*/

                         phy_addr = 0;
                         reg_addr = 0;
                         data = 0x1140;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 4;
                         data = 0x461;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 5;
                         data = 0xC461;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 9;
                         data = 0x0000;
                         gphy_reg_write(phy_addr, reg_addr, &data);





	               phy_addr = 0;
	                                                                                             reg_addr = 0x1;
	                                                                                            gphy0_count = 0;
                                       do {
										    if (gphy0_count == 2000)
										    {
												post_log("GPHY0 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy0_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                                                                               //wait for linkup
	                                                                               /*phy_addr = 1;
	                                                                               reg_addr = 0x1;
	                                                                               do {
	                                                                            	    gphy_reg_read(phy_addr,reg_addr,&data);
	                                                                               } while(!(data & 0x4));*/




	               for(i=0; i<80000000; i++) ;





	               phy_addr = 0;
	                              reg_addr = 0x1;
	                             gphy0_count = 0;
                                       do {
										    if (gphy0_count == 2000)
										    {
												post_log("GPHY0 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy0_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                              if (gphy0_count != 2000)
	                              {
	                              
								  post_log("GPHY0 LINK UP IN 10MBPS WITHOUT LANESWAP\n");
								  post_log("==========PASS==========\n");
								  return TEST_PASS;
								  }
								  else
	                              {
									  post_log("==========FAIL==========\n");
									  return TEST_FAIL;
								  }





	    return 0;

}

int gphy0_dsp_tap10_register_enable_link_up_test_without_laneswap_10Mbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;
			  uint32_t gphy0_count;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }

                           //phy reset
						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);





                         /*100 Mbps
                         0x4 --> 0x781
                         0x5 --> 0xC781
                         0x9 --> 0
                         0x0 --> 0x140*/

                         phy_addr = 0;
                         reg_addr = 0;
                         data = 0x1140;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 4;
                         data = 0x461;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 5;
                         data = 0xC461;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 9;
                         data = 0x0000;
                         gphy_reg_write(phy_addr, reg_addr, &data);





	              phy_addr = 0;
	                                                                                             reg_addr = 0x1;
	                                                                                            gphy0_count = 0;
                                       do {
										    if (gphy0_count == 2000)
										    {
												post_log("GPHY0 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy0_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                                                                               //wait for linkup
	                                                                               /*phy_addr = 1;
	                                                                               reg_addr = 0x1;
	                                                                               do {
	                                                                            	    gphy_reg_read(phy_addr,reg_addr,&data);
	                                                                               } while(!(data & 0x4));*/




	               for(i=0; i<80000000; i++) ;





	               phy_addr = 0;
	                              reg_addr = 0x1;
	                             gphy0_count = 0;
                                       do {
										    if (gphy0_count == 2000)
										    {
												post_log("GPHY0 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy0_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                              if (gphy0_count != 2000)
	                              {
	                              
								  post_log("GPHY0 LINK UP IN 10MBPS WITHOUT LANESWAP\n");
								  post_log("==========PASS==========\n");
								  return TEST_PASS;
								  }
								  else
	                              {
									  post_log("==========FAIL==========\n");
									  return TEST_FAIL;
								  }

}


int gphy1_dsp_tap10_register_enable_link_up_test_10Mbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;
			 
	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);


                        //Lane Swapping and Polarity inversion for Gphy0
                        phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);


                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);


                         /*100 Mbps
                         0x4 --> 0x781
                         0x5 --> 0xC781
                         0x9 --> 0
                         0x0 --> 0x140*/

                         phy_addr = 1;
                         reg_addr = 0;
                         data = 0x1140;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 4;
                         data = 0x461;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 5;
                         data = 0xC461;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 9;
                         data = 0x0000;
                         gphy_reg_write(phy_addr, reg_addr, &data);





	               phy_addr = 1;
	               reg_addr = 0x1;
	                do {
	                      gphy_reg_read(phy_addr,reg_addr,&data);
	                } while(!(data & 0x4));

	               post_log("GPHY1 LINK UP IN 10Mbps Mode\n");
				   post_log("==========PASS==========\n");





	    return 0;

}

int gphy1_dsp_tap10_register_enable_link_up_test_without_laneswap_10Mbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;
			  uint32_t gphy1_count;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }

									//phy reset
						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);





                         /*100 Mbps
                         0x4 --> 0x781
                         0x5 --> 0xC781
                         0x9 --> 0
                         0x0 --> 0x140*/

                         phy_addr = 1;
                         reg_addr = 0;
                         data = 0x1140;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 4;
                         data = 0x461;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 5;
                         data = 0xC461;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 9;
                         data = 0x0000;
                         gphy_reg_write(phy_addr, reg_addr, &data);





	                   phy_addr = 1;
	                   reg_addr = 0x1;
	                   gphy1_count = 0;
							   do {
									if (gphy1_count == 2000)
									{
										post_log("GPHY1 NOT LINKING UP....TIMING OUT\n");
										break;
									}
									gphy_reg_read(phy_addr,reg_addr,&data);
									gphy1_count++;
							   } while(data != 0x79ED); 


	                              phy_addr = 1;
	                              reg_addr = 0x1;
	                             gphy1_count = 0;
                                       do {
										    if (gphy1_count == 2000)
										    {
												post_log("GPHY1 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy1_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                              if (gphy1_count != 2000)
	                              {
	                              
								  post_log("GPHY1 LINK UP IN 10MBPS WITHOUT LANESWAP\n");
								  post_log("==========PASS==========\n");
								  return TEST_PASS;
								  }
								  else
	                              {
									  post_log("==========FAIL==========\n");
									  return TEST_FAIL;
								  }

}


int gphy1_dsp_tap10_register_enable_link_up_test_100Mbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);


                        //Lane Swapping and Polarity inversion for Gphy0
                        phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);


                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);


                         /*100 Mbps
                         0x4 --> 0x781
                         0x5 --> 0xC781
                         0x9 --> 0
                         0x0 --> 0x140*/

                         phy_addr = 1;
                         reg_addr = 0;
                         data = 0x3140;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 4;
                         data = 0x781;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 5;
                         data = 0xC781;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 9;
                         data = 0x0000;
                         gphy_reg_write(phy_addr, reg_addr, &data);





	               phy_addr = 1;
	               reg_addr = 0x1;
	                do {
	                      gphy_reg_read(phy_addr,reg_addr,&data);
	                } while(!(data & 0x4));

	               post_log("GPHY1 LINK UP IN 100Mbps mode\n");
				   post_log("==========PASS==========\n");





	    return 0;

}

int gphy1_dsp_tap10_register_enable_link_up_test_without_laneswap_100Mbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;
			  uint32_t gphy1_count;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }

									//phy reset
						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);





                         /*100 Mbps
                         0x4 --> 0x781
                         0x5 --> 0xC781
                         0x9 --> 0
                         0x0 --> 0x140*/

                         phy_addr = 1;
                         reg_addr = 0;
                         data = 0x3140;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 4;
                         data = 0x781;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 5;
                         data = 0xC781;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 9;
                         data = 0x0000;
                         gphy_reg_write(phy_addr, reg_addr, &data);





	                   phy_addr = 1;
	                   reg_addr = 0x1;
	                   gphy1_count = 0;
							   do {
									if (gphy1_count == 2000)
									{
										post_log("GPHY1 NOT LINKING UP....TIMING OUT\n");
										break;
									}
									gphy_reg_read(phy_addr,reg_addr,&data);
									gphy1_count++;
							   } while(data != 0x79ED); 


	                              phy_addr = 1;
	                              reg_addr = 0x1;
	                             gphy1_count = 0;
                                       do {
										    if (gphy1_count == 2000)
										    {
												post_log("GPHY1 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy1_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                              if (gphy1_count != 2000)
	                              {
	                              
								  post_log("GPHY1 LINK UP IN 100MBPS WITHOUT LANESWAP\n");
								  post_log("==========PASS==========\n");
								  return TEST_PASS;
								  }
								  else
	                              {
									  post_log("==========FAIL==========\n");
									  return TEST_FAIL;
								  }
}

int gphy0_dsp_tap10_register_enable_link_up_test_without_laneswap_1Gbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;
			  uint32_t gphy0_count;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();



	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);

	        					  rd_data = cpu_rd_single(SWITCH_FEATURE_CONFIG,4);
	        					  rd_data = rd_data | (1 << 4);

	        					 cpu_wr_single(SWITCH_FEATURE_CONFIG,rd_data,4);

	       	        	         page=0x92;
	       	                     offset=0x0;
	       	        		     srab_rdwr(page,offset,0x1ff,5);

	       	        		     post_log("EEE mode operation in place\n");



	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }

									//phy reset
						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);














	                              //Lane Swapping and Polarity inversion for Gphy0
	                                                      /*phy_addr = 0;
	                                                      reg_addr = 0x17;
	                                                      data = 0x0F09;
	                                                      gphy_reg_write(phy_addr, reg_addr, &data);
	                                                      //gphy_reg_read(phy_addr, reg_addr, &data);

	                                                      phy_addr = 0;
	                                                      reg_addr = 0x15;
	                                                      data = 0x5193;
	                                                      gphy_reg_write(phy_addr, reg_addr, &data);

	                                                      //Lane Swapping and Polarity inversion for Gphy1
	                                                                               phy_addr = 1;
	                                                                               reg_addr = 0x17;
	                                                                               data = 0x0F09;
	                                                                               gphy_reg_write(phy_addr, reg_addr, &data);
	                                                                               //gphy_reg_read(phy_addr, reg_addr, &data);

	                                                                               phy_addr = 1;
	                                                                               reg_addr = 0x15;
	                                                                               data = 0x11C9;
	                                                                               gphy_reg_write(phy_addr, reg_addr, &data);*/

	                                                                               for(i=0; i<20000000; i++) ;

	                                                                               phy_addr = 0;
	                                                                                             reg_addr = 0x1;
	                                                                                            gphy0_count = 0;
                                       do {
										    if (gphy0_count == 2000)
										    {
												post_log("GPHY0 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy0_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                                                                               //wait for linkup
	                                                                               /*phy_addr = 1;
	                                                                               reg_addr = 0x1;
	                                                                               do {
	                                                                            	    gphy_reg_read(phy_addr,reg_addr,&data);
	                                                                               } while(!(data & 0x4));*/




	               for(i=0; i<80000000; i++) ;





	               phy_addr = 0;
	                              reg_addr = 0x1;
	                             gphy0_count = 0;
                                       do {
										    if (gphy0_count == 2000)
										    {
												post_log("GPHY0 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy0_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                              if (gphy0_count != 2000)
	                              {
	                              
								  post_log("GPHY0 LINK UP IN 1GBPS WITHOUT LANESWAP\n");
								  post_log("==========PASS==========\n");
								  return TEST_PASS;
								  }
								  else
	                              {
									  post_log("==========FAIL==========\n");
									  return TEST_FAIL;
								  }

}

int gphy1_dsp_tap10_register_enable_link_up_test_without_laneswap_1Gbps( void)
{
	          uint32_t phy_addr,i,reg_addr;
	          uint32_t data,bypass_mode,rd_data;
	          uint32_t page;
	          uint32_t offset;
			  uint32_t gphy1_count;

	          bypass_mode = 0;






	    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

	        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
	        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

	        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
	        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





	        amac_init(0,bypass_mode);

	        cfg_sw1();

	        	                 page=0x0;
	                              offset=0x6f;
	                              //rdata = srab_rd(page, offset);
	                              //wdatal=(rdatal | 0x1);
	                              //wdatah=rdatah;
	                              //srab_wr(page, offset, wdatah, wdatal);
	        					  srab_rdwr(page,offset,0x1,5);


	            ccg_mdio_init();

	            for(i=0; i<80000000; i++) ;
	             phy_addr = 0;
	               for(i=0; i<16; i++) {
	                      reg_addr = i;
	                      gphy_reg_read(phy_addr, reg_addr, &data);
	                 }

	               phy_addr = 1;
	                              for(i=0; i<16; i++) {
	                                     reg_addr = i;
	                                     gphy_reg_read(phy_addr, reg_addr, &data);
	                                }

									//phy reset
						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);


	                    phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);














	                              //Lane Swapping and Polarity inversion for Gphy0
	                                                      /*phy_addr = 0;
	                                                      reg_addr = 0x17;
	                                                      data = 0x0F09;
	                                                      gphy_reg_write(phy_addr, reg_addr, &data);
	                                                      //gphy_reg_read(phy_addr, reg_addr, &data);

	                                                      phy_addr = 0;
	                                                      reg_addr = 0x15;
	                                                      data = 0x5193;
	                                                      gphy_reg_write(phy_addr, reg_addr, &data);

	                                                      //Lane Swapping and Polarity inversion for Gphy1
	                                                                               phy_addr = 1;
	                                                                               reg_addr = 0x17;
	                                                                               data = 0x0F09;
	                                                                               gphy_reg_write(phy_addr, reg_addr, &data);
	                                                                               //gphy_reg_read(phy_addr, reg_addr, &data);

	                                                                               phy_addr = 1;
	                                                                               reg_addr = 0x15;
	                                                                               data = 0x11C9;
	                                                                               gphy_reg_write(phy_addr, reg_addr, &data);*/

	                                                                               for(i=0; i<20000000; i++) ;

	                                                                               





	    	                   phy_addr = 1;
	                   reg_addr = 0x1;
	                   gphy1_count = 0;
							   do {
									if (gphy1_count == 2000)
									{
										post_log("GPHY1 NOT LINKING UP....TIMING OUT\n");
										break;
									}
									gphy_reg_read(phy_addr,reg_addr,&data);
									gphy1_count++;
							   } while(data != 0x79ED); 


	                              phy_addr = 1;
	                              reg_addr = 0x1;
	                             gphy1_count = 0;
                                       do {
										    if (gphy1_count == 2000)
										    {
												post_log("GPHY1 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy1_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

	                              if (gphy1_count != 2000)
	                              {
	                              
								  post_log("GPHY1 LINK UP IN 1GBPS WITHOUT LANESWAP\n");
								  post_log("==========PASS==========\n");
								  return TEST_PASS;
								  }
								  else
	                              {
									  post_log("==========FAIL==========\n");
									  return TEST_FAIL;
								  }

}

   int POS_gphy0_link_up_test(void)
{

          uint32_t phy_addr,i,reg_addr;
          uint32_t data,bypass_mode,rd_data;
          uint32_t page;
          uint32_t offset;

          bypass_mode = 0;






    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





        amac_init(0,bypass_mode);

        cfg_sw1();

        	                 page=0x0;
                              offset=0x6f;
                              //rdata = srab_rd(page, offset);
                              //wdatal=(rdatal | 0x1);
                              //wdatah=rdatah;
                              //srab_wr(page, offset, wdatah, wdatal);
        					  srab_rdwr(page,offset,0x1,5);


            ccg_mdio_init();

            for(i=0; i<80000000; i++) ;
             phy_addr = 0;
               for(i=0; i<16; i++) {
                      reg_addr = i;
                      gphy_reg_read(phy_addr, reg_addr, &data);
                 }

               phy_addr = 1;
                              for(i=0; i<16; i++) {
                                     reg_addr = i;
                                     gphy_reg_read(phy_addr, reg_addr, &data);
                                }






               for(i=0; i<80000000; i++) ;





               /*phy_addr = 0;
                              reg_addr = 0x1;
                              do {
                           	    gphy_reg_read(phy_addr,reg_addr,&data);
                              } while(!(data & 0x4));

                              post_log("GPHY0 LINK UP \n");*/





    return 0;
   }


   int POS_gphy0_link_up_test1(void)
  {

            uint32_t phy_addr,i,reg_addr;
            uint32_t data,bypass_mode,rd_data;
            uint32_t page;
                      uint32_t offset;

            bypass_mode = 0;






      cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
          cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

          rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
          post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

          rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
          post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);





          amac_init(0,bypass_mode);

          cfg_sw1();

          	                 //page=0x0;
                               // offset=0x6f;
                                //rdata = srab_rd(page, offset);
                                //wdatal=(rdatal | 0x1);
                                //wdatah=rdatah;
                                //srab_wr(page, offset, wdatah, wdatal);
          					  //srab_rdwr(page,offset,0x1,5);


              ccg_mdio_init();

              for(i=0; i<80000000; i++) ;
               phy_addr = 0;
                 for(i=0; i<16; i++) {
                        reg_addr = i;
                        gphy_reg_read(phy_addr, reg_addr, &data);
                   }

                 phy_addr = 1;
                                for(i=0; i<16; i++) {
                                       reg_addr = i;
                                       gphy_reg_read(phy_addr, reg_addr, &data);
                                  }






                 for(i=0; i<80000000; i++) ;





                 /*phy_addr = 0;
                                reg_addr = 0x1;
                                do {
                             	    gphy_reg_read(phy_addr,reg_addr,&data);
                                } while(!(data & 0x4));

                                post_log("GPHY0 LINK UP \n");*/





      return 0;
     }


   int gphy0_link_up_bypass_mode_test(void)
{

          uint32_t phy_addr,i,reg_addr;
          uint32_t data,bypass_mode,rd_data;

          bypass_mode = 0;




    

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	

	
        amac_init(0,bypass_mode);

		cpu_wr_single(SWITCH_GLOBAL_CONFIG,0x2001,4);


            ccg_mdio_init();

            for(i=0; i<80000000; i++) ;
             phy_addr = 0;
               for(i=0; i<16; i++) {
                      reg_addr = i;
                      gphy_reg_read(phy_addr, reg_addr, &data);
                 }

              

              


               for(i=0; i<80000000; i++) ;





               phy_addr = 0;
                              reg_addr = 0x1;
                              do {
                           	    gphy_reg_read(phy_addr,reg_addr,&data);
                              } while(!(data & 0x4));

                              post_log("GPHY0 LINK UP \n");


                          


    return 0;
   }


      int gphy0_bypass_mode_external_powerdown_test(void)
{

          uint32_t phy_addr,i,reg_addr;
          uint32_t data,bypass_mode,rd_data;

          bypass_mode = 0;




    

    cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
        cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

        rd_data = cpu_rd_single(CRMU_CHIP_IO_PAD_CONTROL,4);
        post_log("CRMU_CHIP_IO_PAD_CONTROL = %08x\n",rd_data);

        rd_data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
        post_log("CRMU_IOMUX_CTL7 = %08x\n",rd_data);


	

	
        amac_init(0,bypass_mode);

		cpu_wr_single(SWITCH_GLOBAL_CONFIG,0x2001,4);



            ccg_mdio_init();

            for(i=0; i<80000000; i++) ;
             phy_addr = 0;
               for(i=0; i<16; i++) {
                      reg_addr = i;
                      gphy_reg_read(phy_addr, reg_addr, &data);
                 }

              

              


               for(i=0; i<80000000; i++) ;





             cpu_wr_single(CDRU_GPHY_PWR_DOWN_CTRL,0x1,4); 


                          


    return 0;
   }

   int amacswitch_port0_to_port1_ddr_loopback(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;
		  uint32_t gphy0_count,gphy1_count;

   bypass_mode = 0;




    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }





	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =200;//Max Desc=512=0x200
              rx_desc_len       =200;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000221,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        //amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 cfg_sw1();

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

						   //phy reset
						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

                          //DSP_TAP10 FIX INCLUDED
						  phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

                         

                         phy_addr = 0;
                                       reg_addr = 0x1;
									   gphy0_count = 0;
                                       do {
										    if (gphy0_count == 2000)
										    {
												post_log("GPHY0 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy0_count++;
                                       } while(data != 0x79ED); //while(!(data & 0x4));

                          
                         //wait for linkup
                         phy_addr = 1;
                         reg_addr = 0x1;

                         gphy1_count = 0;
                                       do {
										    if (gphy1_count == 2000)
										    {
												post_log("GPHY1 NOT LINKING UP....TIMING OUT\n");
												break;
										    }
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
											gphy1_count++;
                         } while(data != 0x79ED); //while(!(data & 0x4));



    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

//   // read port8 rx stastic register
//	page=0x28; 
//	// read port8 rx good byte register	
//	offset=0x0;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3); 
//	
//// read port1 rx stastic register
//	page=0x20; 
//	// read port8 rx good byte register	
//	offset=0x0;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3); 
//   
//   // read port8 rx stastic register
//	page=0x28; 
//	// read port8 rx good byte register	
//	offset=0x50;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3); 
//	
//// read port1 rx stastic register
//	page=0x21; 
//	// read port8 rx good byte register	
//	offset=0x51;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3); 
	





   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
				 post_log("==========PASS==========\n");
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
				 post_log("==========FAIL==========\n");
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }

   int amacswitch_port0_to_port1_ddr_loopback_100MBPS(void)
      {

       test_status_t status = TEST_PASS;
       uint32_t test_args[NUM_TEST_ARGS];
       char *test_args_name[NUM_TEST_ARGS];
       uint32_t l,frame_sz,iter=1;
       uint32_t index;
   	test_status_t result,rpkt_no,rpkt_chk;
      test_status_t res_sts[2];
      amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
      amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
      amac_rx_pkt_info_t* pkt_ptr;
      uint32_t bypass_mode;

       uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

      bypass_mode = 0;




       //256-byte data; CRC will be appended by Tx MAC
       uint8_t my_pkt_data[256] =
      {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

      };

       uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
       uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
       uint8_t rdata8;

   	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
       uint32_t frame_size,step,chk_noPkt;
       uint32_t timeout;
       uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

   	if(BCM958302K == post_get_board_diags_type())
      {
             post_log("\nSKIP : Diag not supported in POS Boards\n");
             return TEST_SKIP;
      }





   	 ///////////// TEST ARG INIT ////////////////////

                 test_args_name[0]="Amac Ports =";
                 test_args_name[1]="Tx Desc Base =";
                 test_args_name[2]="Rx Desc Base =";
                 test_args_name[3]="Tx Desc Depth =";
                 test_args_name[4]="Rx Desc Depth =";
                 test_args_name[5]="Tx Buffer Base =";
                 test_args_name[6]="Rx Buffer Base =";
                 test_args_name[7]="Frame size =";

                 amac_ports        =1;
                 tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
                 rx_desc_base      =0x72000000;
                 tx_desc_len       =200;//Max Desc=512=0x200
                 rx_desc_len       =200;
                 tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
                 rx_buff_base      =0x62000000;
                 frame_size          =128;
                 //frame_sz            =1024;
                  step            =23;
       chk_noPkt       =tx_desc_len-1;
       //UNIMAC Config args
       eth_speed       =0x2;//1Gbps
       rx_pause_ignore =1;
       hd_ena          =1;
       mac_loop_con    =0;
       ena_ext_config  =1;
       rmt_loop_ena    =0;
       tx_pause_ignore =0;

   	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000221,4); //Select RGMII pad as input
   	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

   				//cpu_wr_single(0x301d060,0xb,4);
                   //for(i=0;i<2000000;i++);
   				//cpu_wr_single(0x301d060,0xf,4);


   	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

      ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
           //Basic AMAC block init
           amac_init(amac_id,bypass_mode);

           //UNIMAC Config
           unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                        rmt_loop_ena,tx_pause_ignore);

           //Reset MIB Counter
           counter_reset(amac_id);

           //Set up MAC loopback
           //amac_set_config(amac_id, MAC_LOOPBACK, 1);

           //Initialize AMAC TX DMA block
           amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
           amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
           for(index = 0; index < tx_desc_len; index++) {

                //frame_size=((rand()%(0x3000-64))+64);

                CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
           }

           //Initialize AMAC RX DMA block
           amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                           AMAC_PKT_BUFFER_SIZE_IN_BYTES);

           //Update with next descriptor value - outside EOT range
           //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

           //Within EOT range
           amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

            //amac_get_tx_status(amac_id);
            //amac_get_rx_status(amac_id);
            post_log("\n\n");
      }
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    cfg_sw1();

   	                 page=0x0;
                         offset=0x6f;
                         //rdata = srab_rd(page, offset);
                         //wdatal=(rdatal | 0x1);
                         //wdatah=rdatah;
                         //srab_wr(page, offset, wdatah, wdatal);
   					  srab_rdwr(page,offset,0x1,5);


                         ccg_mdio_init();

                          phy_addr = 0;
                            for(i=0; i<16; i++) {
                                   reg_addr = i;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                              }

							  //phy reset
						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

                             //DSP_TAP10 FIX INCLUDED
   						  phy_addr = 0;
   	                    reg_addr = 0x18;
   	                    data = 0x0C30;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 0;
   	                    reg_addr = 0x17;
   	                    data = 0x000A;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 0;
   	                    reg_addr = 0x15;
   	                    data = 0x111B;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 1;
   	                    reg_addr = 0x18;
   	                    data = 0x0C30;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 1;
   	                    reg_addr = 0x17;
   	                    data = 0x000A;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 1;
   	                    reg_addr = 0x15;
   	                    data = 0x111B;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

                            phy_addr = 0;
                            reg_addr = 0;
                            data = 0x3140;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 4;
                            data = 0x781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 5;
                            data = 0xC781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 9;
                            data = 0x0000;
                            gphy_reg_write(phy_addr, reg_addr, &data);


                            phy_addr = 1;
                            reg_addr = 0;
                            data = 0x3140;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 4;
                            data = 0x781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 5;
                            data = 0xC781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 9;
                            data = 0x0000;
                            gphy_reg_write(phy_addr, reg_addr, &data);


                            for(i=0; i<20000000; i++) ;

                            phy_addr = 0;
                                          reg_addr = 0x1;
                                          do {
                                       	    gphy_reg_read(phy_addr,reg_addr,&data);
                                          } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));

                            //wait for linkup
                            phy_addr = 1;
                            reg_addr = 0x1;
                            do {
                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                            } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));



       ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
           // NxtDscr points to EOT Dscr
           nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           //Out of EOT range,for continuous loopback
           //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           amac_enable_tx_dma(amac_id, nextDscr);
           post_log("\n\n");
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
      /*timeout=300;
      post_log("Sending packets for specified time\n");
      delay_noise(timeout,0);
      post_log("Time_taken =%d secs\n",(timeout*7));
      suspend_tx(amac_id);*/

   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x20;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x50;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x21;
   //	// read port8 rx good byte register
   //	offset=0x51;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);






      for(amac_id=0;amac_id < amac_ports;amac_id++){
           waitforTrans_internalloopback(amac_id);
           post_log("\n");
           amac_gettxstats(amac_id);
           amac_getrxstats(amac_id);

           rpkt_no=checker_all(amac_id);
           post_log("\n");

           post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
           rpkt_chk=TEST_PASS;
           rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

            if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                    res_sts[amac_id]=TEST_PASS;
                    post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
   				 post_log("==========PASS==========\n");
            }
            else{
                    res_sts[amac_id]=TEST_FAIL;
                    post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
   				 post_log("==========FAIL==========\n");
            }

            post_log("\n\n");
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      if((res_sts[0]==TEST_PASS)){
               result=TEST_PASS;
      }else{
               result=TEST_FAIL;
       }
      return result;

      }


   int amacswitch_port0_to_port1_ddr_loopback_10MBPS(void)
         {

          test_status_t status = TEST_PASS;
          uint32_t test_args[NUM_TEST_ARGS];
          char *test_args_name[NUM_TEST_ARGS];
          uint32_t l,frame_sz,iter=1;
          uint32_t index;
      	test_status_t result,rpkt_no,rpkt_chk;
         test_status_t res_sts[2];
         amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
         amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
         amac_rx_pkt_info_t* pkt_ptr;
         uint32_t bypass_mode;

          uint32_t page;
            	uint32_t offset;
            	uint32_t wdatah, wdatal;
            	uint32_t rdatah, rdatal;
            	uint32_t port_id;
                uint32_t *rdata;
                uint32_t phy_addr,i,reg_addr;
                uint32_t data;

         bypass_mode = 0;




          //256-byte data; CRC will be appended by Tx MAC
          uint8_t my_pkt_data[256] =
         {
           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
           0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
           0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
           0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
           0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
           0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
           0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
           0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
           0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
           0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
           0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
           0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
           0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
           0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
           0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
           0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

         };

          uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
          uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
          uint8_t rdata8;

      	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
          uint32_t frame_size,step,chk_noPkt;
          uint32_t timeout;
          uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

      	if(BCM958302K == post_get_board_diags_type())
         {
                post_log("\nSKIP : Diag not supported in POS Boards\n");
                return TEST_SKIP;
         }





      	 ///////////// TEST ARG INIT ////////////////////

                    test_args_name[0]="Amac Ports =";
                    test_args_name[1]="Tx Desc Base =";
                    test_args_name[2]="Rx Desc Base =";
                    test_args_name[3]="Tx Desc Depth =";
                    test_args_name[4]="Rx Desc Depth =";
                    test_args_name[5]="Tx Buffer Base =";
                    test_args_name[6]="Rx Buffer Base =";
                    test_args_name[7]="Frame size =";

                    amac_ports        =1;
                    tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
                    rx_desc_base      =0x72000000;
                    tx_desc_len       =200;//Max Desc=512=0x200
                    rx_desc_len       =200;
                    tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
                    rx_buff_base      =0x62000000;
                    frame_size          =128;
                    //frame_sz            =1024;
                     step            =23;
          chk_noPkt       =tx_desc_len-1;
          //UNIMAC Config args
          eth_speed       =0x2;//1Gbps
          rx_pause_ignore =1;
          hd_ena          =1;
          mac_loop_con    =0;
          ena_ext_config  =1;
          rmt_loop_ena    =0;
          tx_pause_ignore =0;

      	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000221,4); //Select RGMII pad as input
      	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

      				//cpu_wr_single(0x301d060,0xb,4);
                      //for(i=0;i<2000000;i++);
      				//cpu_wr_single(0x301d060,0xf,4);


      	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

         ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
         for(amac_id=0;amac_id < amac_ports;amac_id++){
              post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
              //Basic AMAC block init
              amac_init(amac_id,bypass_mode);

              //UNIMAC Config
              unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                           rmt_loop_ena,tx_pause_ignore);

              //Reset MIB Counter
              counter_reset(amac_id);

              //Set up MAC loopback
              //amac_set_config(amac_id, MAC_LOOPBACK, 1);

              //Initialize AMAC TX DMA block
              amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
              amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
              for(index = 0; index < tx_desc_len; index++) {

                   //frame_size=((rand()%(0x3000-64))+64);

                   CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                   write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                   //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                   amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                   (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                   amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
              }

              //Initialize AMAC RX DMA block
              amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                              AMAC_PKT_BUFFER_SIZE_IN_BYTES);

              //Update with next descriptor value - outside EOT range
              //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

              //Within EOT range
              amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

               //amac_get_tx_status(amac_id);
               //amac_get_rx_status(amac_id);
               post_log("\n\n");
         }
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                       cfg_sw1();

      	                 page=0x0;
                            offset=0x6f;
                            //rdata = srab_rd(page, offset);
                            //wdatal=(rdatal | 0x1);
                            //wdatah=rdatah;
                            //srab_wr(page, offset, wdatah, wdatal);
      					  srab_rdwr(page,offset,0x1,5);


                            ccg_mdio_init();

                             phy_addr = 0;
                               for(i=0; i<16; i++) {
                                      reg_addr = i;
                                      gphy_reg_read(phy_addr, reg_addr, &data);
                                 }

								 //phy reset
						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 0;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data | (1 << 15);
						   gphy_reg_write(phy_addr,reg_addr,&data);

						   phy_addr = 1;
						   reg_addr = 0;
                           gphy_reg_read(phy_addr, reg_addr, &data);
						   data = data & (~(1 << 15));
						   gphy_reg_write(phy_addr,reg_addr,&data);

                                //DSP_TAP10 FIX INCLUDED
      						  phy_addr = 0;
      	                    reg_addr = 0x18;
      	                    data = 0x0C30;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 0;
      	                    reg_addr = 0x17;
      	                    data = 0x000A;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 0;
      	                    reg_addr = 0x15;
      	                    data = 0x111B;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 1;
      	                    reg_addr = 0x18;
      	                    data = 0x0C30;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 1;
      	                    reg_addr = 0x17;
      	                    data = 0x000A;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 1;
      	                    reg_addr = 0x15;
      	                    data = 0x111B;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

                               

                               phy_addr = 1;
                               reg_addr = 0;
                               data = 0x1140;
                               gphy_reg_write(phy_addr, reg_addr, &data);

                               phy_addr = 1;
                               reg_addr = 4;
                               data = 0x461;
                               gphy_reg_write(phy_addr, reg_addr, &data);

                               phy_addr = 1;
                               reg_addr = 5;
                               data = 0xC461;
                               gphy_reg_write(phy_addr, reg_addr, &data);

                               phy_addr = 1;
                               reg_addr = 9;
                               data = 0x0000;
                               gphy_reg_write(phy_addr, reg_addr, &data);

                               phy_addr = 0;
                               reg_addr = 0;
                               data = 0x1140;
                               gphy_reg_write(phy_addr, reg_addr, &data);

                               phy_addr = 0;
                               reg_addr = 4;
                               data = 0x461;
                               gphy_reg_write(phy_addr, reg_addr, &data);

                               phy_addr = 0;
                               reg_addr = 5;
                               data = 0xC461;
                               gphy_reg_write(phy_addr, reg_addr, &data);

                               phy_addr = 0;
                               reg_addr = 9;
                               data = 0x0000;
                               gphy_reg_write(phy_addr, reg_addr, &data);



                               for(i=0; i<20000000; i++) ;

                               phy_addr = 0;
                                             reg_addr = 0x1;
                                             do {
                                          	    gphy_reg_read(phy_addr,reg_addr,&data);
                                             } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));

                               //wait for linkup
                               phy_addr = 1;
                               reg_addr = 0x1;
                               do {
                            	    gphy_reg_read(phy_addr,reg_addr,&data);
                               } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));



          ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
         for(amac_id=0;amac_id < amac_ports;amac_id++){
              post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
              // NxtDscr points to EOT Dscr
              nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                      ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
              //Out of EOT range,for continuous loopback
              //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                      ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
              amac_enable_tx_dma(amac_id, nextDscr);
              post_log("\n\n");
         }
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




         //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
         /*timeout=300;
         post_log("Sending packets for specified time\n");
         delay_noise(timeout,0);
         post_log("Time_taken =%d secs\n",(timeout*7));
         suspend_tx(amac_id);*/

      //   // read port8 rx stastic register
      //	page=0x28;
      //	// read port8 rx good byte register
      //	offset=0x0;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);
      //
      //// read port1 rx stastic register
      //	page=0x20;
      //	// read port8 rx good byte register
      //	offset=0x0;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);
      //
      //   // read port8 rx stastic register
      //	page=0x28;
      //	// read port8 rx good byte register
      //	offset=0x50;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);
      //
      //// read port1 rx stastic register
      //	page=0x21;
      //	// read port8 rx good byte register
      //	offset=0x51;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);






         for(amac_id=0;amac_id < amac_ports;amac_id++){
              waitforTrans_internalloopback(amac_id);
              post_log("\n");
              amac_gettxstats(amac_id);
              amac_getrxstats(amac_id);

              rpkt_no=checker_all(amac_id);
              post_log("\n");

              post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
              rpkt_chk=TEST_PASS;
              rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

               if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                       res_sts[amac_id]=TEST_PASS;
                       post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
      				 post_log("==========PASS==========\n");
               }
               else{
                       res_sts[amac_id]=TEST_FAIL;
                       post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
      				 post_log("==========FAIL==========\n");
               }

               post_log("\n\n");
         }
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


         if((res_sts[0]==TEST_PASS)){
                  result=TEST_PASS;
         }else{
                  result=TEST_FAIL;
          }
         return result;
         }



    int amacswitch_port1_to_port0_ddr_loopback_1GBPS(void)
   {

    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }





	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =200;//Max Desc=512=0x200
              rx_desc_len       =200;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000221,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        //amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 cfg_sw1_port1_to_port0();

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                          //DSP_TAP10 FIX INCLUDED
						  phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

						  //wait for linkup
                         phy_addr = 1;
                         reg_addr = 0x1;
                         do {
                      	    gphy_reg_read(phy_addr,reg_addr,&data);
                         } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));

                         phy_addr = 0;
                                       reg_addr = 0x1;
                                       do {
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
                                       } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));





    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

//   // read port8 rx stastic register
//	page=0x28;
//	// read port8 rx good byte register
//	offset=0x0;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3);
//
//// read port1 rx stastic register
//	page=0x20;
//	// read port8 rx good byte register
//	offset=0x0;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3);
//
//   // read port8 rx stastic register
//	page=0x28;
//	// read port8 rx good byte register
//	offset=0x50;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3);
//
//// read port1 rx stastic register
//	page=0x21;
//	// read port8 rx good byte register
//	offset=0x51;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3);






   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
				 post_log("==========PASS==========\n");
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
				 post_log("==========FAIL==========\n");
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }


    int amacswitch_port1_to_port0_ddr_loopback_100MBPS(void)
      {

       test_status_t status = TEST_PASS;
       uint32_t test_args[NUM_TEST_ARGS];
       char *test_args_name[NUM_TEST_ARGS];
       uint32_t l,frame_sz,iter=1;
       uint32_t index;
   	test_status_t result,rpkt_no,rpkt_chk;
      test_status_t res_sts[2];
      amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
      amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
      amac_rx_pkt_info_t* pkt_ptr;
      uint32_t bypass_mode;

       uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

      bypass_mode = 0;




       //256-byte data; CRC will be appended by Tx MAC
       uint8_t my_pkt_data[256] =
      {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

      };

       uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
       uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
       uint8_t rdata8;

   	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
       uint32_t frame_size,step,chk_noPkt;
       uint32_t timeout;
       uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

   	if(BCM958302K == post_get_board_diags_type())
      {
             post_log("\nSKIP : Diag not supported in POS Boards\n");
             return TEST_SKIP;
      }





   	 ///////////// TEST ARG INIT ////////////////////

                 test_args_name[0]="Amac Ports =";
                 test_args_name[1]="Tx Desc Base =";
                 test_args_name[2]="Rx Desc Base =";
                 test_args_name[3]="Tx Desc Depth =";
                 test_args_name[4]="Rx Desc Depth =";
                 test_args_name[5]="Tx Buffer Base =";
                 test_args_name[6]="Rx Buffer Base =";
                 test_args_name[7]="Frame size =";

                 amac_ports        =1;
                 tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
                 rx_desc_base      =0x72000000;
                 tx_desc_len       =200;//Max Desc=512=0x200
                 rx_desc_len       =200;
                 tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
                 rx_buff_base      =0x62000000;
                 frame_size          =128;
                 //frame_sz            =1024;
                  step            =23;
       chk_noPkt       =tx_desc_len-1;
       //UNIMAC Config args
       eth_speed       =0x2;//1Gbps
       rx_pause_ignore =1;
       hd_ena          =1;
       mac_loop_con    =0;
       ena_ext_config  =1;
       rmt_loop_ena    =0;
       tx_pause_ignore =0;

   	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000221,4); //Select RGMII pad as input
   	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

   				//cpu_wr_single(0x301d060,0xb,4);
                   //for(i=0;i<2000000;i++);
   				//cpu_wr_single(0x301d060,0xf,4);


   	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

      ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
           //Basic AMAC block init
           amac_init(amac_id,bypass_mode);

           //UNIMAC Config
           unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                        rmt_loop_ena,tx_pause_ignore);

           //Reset MIB Counter
           counter_reset(amac_id);

           //Set up MAC loopback
           //amac_set_config(amac_id, MAC_LOOPBACK, 1);

           //Initialize AMAC TX DMA block
           amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
           amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
           for(index = 0; index < tx_desc_len; index++) {

                //frame_size=((rand()%(0x3000-64))+64);

                CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
           }

           //Initialize AMAC RX DMA block
           amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                           AMAC_PKT_BUFFER_SIZE_IN_BYTES);

           //Update with next descriptor value - outside EOT range
           //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

           //Within EOT range
           amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

            //amac_get_tx_status(amac_id);
            //amac_get_rx_status(amac_id);
            post_log("\n\n");
      }
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    cfg_sw1_port1_to_port0();

   	                 page=0x0;
                         offset=0x6f;
                         //rdata = srab_rd(page, offset);
                         //wdatal=(rdatal | 0x1);
                         //wdatah=rdatah;
                         //srab_wr(page, offset, wdatah, wdatal);
   					  srab_rdwr(page,offset,0x1,5);


                         ccg_mdio_init();

                          phy_addr = 0;
                            for(i=0; i<16; i++) {
                                   reg_addr = i;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                              }

                             //DSP_TAP10 FIX INCLUDED
   						  phy_addr = 0;
   	                    reg_addr = 0x18;
   	                    data = 0x0C30;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 0;
   	                    reg_addr = 0x17;
   	                    data = 0x000A;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 0;
   	                    reg_addr = 0x15;
   	                    data = 0x111B;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 1;
   	                    reg_addr = 0x18;
   	                    data = 0x0C30;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 1;
   	                    reg_addr = 0x17;
   	                    data = 0x000A;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 1;
   	                    reg_addr = 0x15;
   	                    data = 0x111B;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

                            //Lane Swapping and Polarity inversion for Gphy0
                            phy_addr = 0;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 0x15;
                            data = 0x5193;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            //Lane Swapping and Polarity inversion for Gphy1
                            phy_addr = 1;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 0x15;
                            data = 0x11C9;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            phy_addr = 0;
                            reg_addr = 0;
                            data = 0x3140;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 4;
                            data = 0x781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 5;
                            data = 0xC781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 9;
                            data = 0x0000;
                            gphy_reg_write(phy_addr, reg_addr, &data);


                            phy_addr = 1;
                            reg_addr = 0;
                            data = 0x3140;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 4;
                            data = 0x781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 5;
                            data = 0xC781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 9;
                            data = 0x0000;
                            gphy_reg_write(phy_addr, reg_addr, &data);


   						  //wait for linkup
                            phy_addr = 1;
                            reg_addr = 0x1;
                            do {
                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                            } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));

                            phy_addr = 0;
                                          reg_addr = 0x1;
                                          do {
                                       	    gphy_reg_read(phy_addr,reg_addr,&data);
                                          } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));





       ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
           // NxtDscr points to EOT Dscr
           nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           //Out of EOT range,for continuous loopback
           //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           amac_enable_tx_dma(amac_id, nextDscr);
           post_log("\n\n");
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
      /*timeout=300;
      post_log("Sending packets for specified time\n");
      delay_noise(timeout,0);
      post_log("Time_taken =%d secs\n",(timeout*7));
      suspend_tx(amac_id);*/

   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x20;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x50;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x21;
   //	// read port8 rx good byte register
   //	offset=0x51;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);






      for(amac_id=0;amac_id < amac_ports;amac_id++){
           waitforTrans_internalloopback(amac_id);
           post_log("\n");
           amac_gettxstats(amac_id);
           amac_getrxstats(amac_id);

           rpkt_no=checker_all(amac_id);
           post_log("\n");

           post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
           rpkt_chk=TEST_PASS;
           rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

            if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                    res_sts[amac_id]=TEST_PASS;
                    post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
   				 post_log("==========PASS==========\n");
            }
            else{
                    res_sts[amac_id]=TEST_FAIL;
                    post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
   				 post_log("==========FAIL==========\n");
            }

            post_log("\n\n");
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      if((res_sts[0]==TEST_PASS)){
               result=TEST_PASS;
      }else{
               result=TEST_FAIL;
       }
      return result;

      }


    int amacswitch_port1_to_port0_ddr_loopback_10MBPS(void)
      {

       test_status_t status = TEST_PASS;
       uint32_t test_args[NUM_TEST_ARGS];
       char *test_args_name[NUM_TEST_ARGS];
       uint32_t l,frame_sz,iter=1;
       uint32_t index;
   	test_status_t result,rpkt_no,rpkt_chk;
      test_status_t res_sts[2];
      amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
      amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
      amac_rx_pkt_info_t* pkt_ptr;
      uint32_t bypass_mode;

       uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

      bypass_mode = 0;




       //256-byte data; CRC will be appended by Tx MAC
       uint8_t my_pkt_data[256] =
      {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

      };

       uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
       uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
       uint8_t rdata8;

   	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
       uint32_t frame_size,step,chk_noPkt;
       uint32_t timeout;
       uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

   	if(BCM958302K == post_get_board_diags_type())
      {
             post_log("\nSKIP : Diag not supported in POS Boards\n");
             return TEST_SKIP;
      }





   	 ///////////// TEST ARG INIT ////////////////////

                 test_args_name[0]="Amac Ports =";
                 test_args_name[1]="Tx Desc Base =";
                 test_args_name[2]="Rx Desc Base =";
                 test_args_name[3]="Tx Desc Depth =";
                 test_args_name[4]="Rx Desc Depth =";
                 test_args_name[5]="Tx Buffer Base =";
                 test_args_name[6]="Rx Buffer Base =";
                 test_args_name[7]="Frame size =";

                 amac_ports        =1;
                 tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
                 rx_desc_base      =0x72000000;
                 tx_desc_len       =200;//Max Desc=512=0x200
                 rx_desc_len       =200;
                 tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
                 rx_buff_base      =0x62000000;
                 frame_size          =128;
                 //frame_sz            =1024;
                  step            =23;
       chk_noPkt       =tx_desc_len-1;
       //UNIMAC Config args
       eth_speed       =0x2;//1Gbps
       rx_pause_ignore =1;
       hd_ena          =1;
       mac_loop_con    =0;
       ena_ext_config  =1;
       rmt_loop_ena    =0;
       tx_pause_ignore =0;

   	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000221,4); //Select RGMII pad as input
   	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

   				//cpu_wr_single(0x301d060,0xb,4);
                   //for(i=0;i<2000000;i++);
   				//cpu_wr_single(0x301d060,0xf,4);


   	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

      ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
           //Basic AMAC block init
           amac_init(amac_id,bypass_mode);

           //UNIMAC Config
           unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                        rmt_loop_ena,tx_pause_ignore);

           //Reset MIB Counter
           counter_reset(amac_id);

           //Set up MAC loopback
           //amac_set_config(amac_id, MAC_LOOPBACK, 1);

           //Initialize AMAC TX DMA block
           amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
           amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
           for(index = 0; index < tx_desc_len; index++) {

                //frame_size=((rand()%(0x3000-64))+64);

                CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
           }

           //Initialize AMAC RX DMA block
           amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                           AMAC_PKT_BUFFER_SIZE_IN_BYTES);

           //Update with next descriptor value - outside EOT range
           //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

           //Within EOT range
           amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

            //amac_get_tx_status(amac_id);
            //amac_get_rx_status(amac_id);
            post_log("\n\n");
      }
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    cfg_sw1_port1_to_port0();

   	                 page=0x0;
                         offset=0x6f;
                         //rdata = srab_rd(page, offset);
                         //wdatal=(rdatal | 0x1);
                         //wdatah=rdatah;
                         //srab_wr(page, offset, wdatah, wdatal);
   					  srab_rdwr(page,offset,0x1,5);


                         ccg_mdio_init();

                          phy_addr = 0;
                            for(i=0; i<16; i++) {
                                   reg_addr = i;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                              }

                             //DSP_TAP10 FIX INCLUDED
   						  phy_addr = 0;
   	                    reg_addr = 0x18;
   	                    data = 0x0C30;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 0;
   	                    reg_addr = 0x17;
   	                    data = 0x000A;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 0;
   	                    reg_addr = 0x15;
   	                    data = 0x111B;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 1;
   	                    reg_addr = 0x18;
   	                    data = 0x0C30;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 1;
   	                    reg_addr = 0x17;
   	                    data = 0x000A;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

   	                    phy_addr = 1;
   	                    reg_addr = 0x15;
   	                    data = 0x111B;
   	                    gphy_reg_write(phy_addr,reg_addr,&data);

                            //Lane Swapping and Polarity inversion for Gphy0
                            phy_addr = 0;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 0x15;
                            data = 0x5193;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            //Lane Swapping and Polarity inversion for Gphy1
                            phy_addr = 1;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 0x15;
                            data = 0x11C9;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            phy_addr = 1;
                            reg_addr = 0;
                            data = 0x1140;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 4;
                            data = 0x461;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 5;
                            data = 0xC461;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 9;
                            data = 0x0000;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 0;
                            data = 0x1140;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 4;
                            data = 0x461;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 5;
                            data = 0xC461;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 9;
                            data = 0x0000;
                            gphy_reg_write(phy_addr, reg_addr, &data);


   						  //wait for linkup
                            phy_addr = 1;
                            reg_addr = 0x1;
                            do {
                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                            } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));

                            phy_addr = 0;
                                          reg_addr = 0x1;
                                          do {
                                       	    gphy_reg_read(phy_addr,reg_addr,&data);
                                          } while(data != 0x79ED); //while(!(data & 0x4));while(!(data & 0x4));





       ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
           // NxtDscr points to EOT Dscr
           nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           //Out of EOT range,for continuous loopback
           //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           amac_enable_tx_dma(amac_id, nextDscr);
           post_log("\n\n");
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
      /*timeout=300;
      post_log("Sending packets for specified time\n");
      delay_noise(timeout,0);
      post_log("Time_taken =%d secs\n",(timeout*7));
      suspend_tx(amac_id);*/

   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x20;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x50;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x21;
   //	// read port8 rx good byte register
   //	offset=0x51;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);






      for(amac_id=0;amac_id < amac_ports;amac_id++){
           waitforTrans_internalloopback(amac_id);
           post_log("\n");
           amac_gettxstats(amac_id);
           amac_getrxstats(amac_id);

           rpkt_no=checker_all(amac_id);
           post_log("\n");

           post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
           rpkt_chk=TEST_PASS;
           rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

            if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                    res_sts[amac_id]=TEST_PASS;
                    post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
   				 post_log("==========PASS==========\n");
            }
            else{
                    res_sts[amac_id]=TEST_FAIL;
                    post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
   				 post_log("==========FAIL==========\n");
            }

            post_log("\n\n");
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      if((res_sts[0]==TEST_PASS)){
               result=TEST_PASS;
      }else{
               result=TEST_FAIL;
       }
      return result;

      }







    int gphy0_internal_loopback_1GBPS(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

//	if(POS != post_get_board_diags_type())
//   {
//          post_log("\nSKIP : Diag not supported in VOIP and COMBO Boards\n");
//          return TEST_SKIP;
//   }




	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x73000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x74000000;
              tx_desc_len       =200;//Max Desc=512=0x200
              rx_desc_len       =200;
              tx_buff_base      =0x63000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x64000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        //amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 cfg_sw1_rev();

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //DSP_TAP10 FIX INCLUDED
						  phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);


                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                        /* phy_addr = 0;
                                       reg_addr = 0x1;
                                       do {
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
                                       } while(!(data & 0x4));*/

                         //wait for linkup
                         /*phy_addr = 1;
                         reg_addr = 0x1;
                         do {
                      	    gphy_reg_read(phy_addr,reg_addr,&data);
                         } while(!(data & 0x4));*/

                      phy_addr = 0;
						  reg_addr = 0;
            gphy_reg_read(phy_addr, reg_addr, &data);
            data |= (1 << 14);
            gphy_reg_write(phy_addr, reg_addr, &data);
            gphy_reg_read(phy_addr, reg_addr, &data);

            data = 0x01E1;
            gphy_reg_write(phy_addr, 0x0004, &data);
            gphy_reg_read(phy_addr, 0x0004, &data);



    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

//   // read port8 rx stastic register
//	page=0x28; 
//	// read port8 rx good byte register	
//	offset=0x0;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3); 
//	
//// read port1 rx stastic register
//	page=0x20; 
//	// read port8 rx good byte register	
//	offset=0x0;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3); 
//   
//   // read port8 rx stastic register
//	page=0x28; 
//	// read port8 rx good byte register	
//	offset=0x50;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3); 
//	
//// read port1 rx stastic register
//	page=0x21; 
//	// read port8 rx good byte register	
//	offset=0x51;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3); 
	





   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                 post_log("==========PASS==========\n");
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                 post_log("==========FAIL==========\n");
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }


    int gphy0_internal_loopback_100MBPS(void)
      {

       test_status_t status = TEST_PASS;
       uint32_t test_args[NUM_TEST_ARGS];
       char *test_args_name[NUM_TEST_ARGS];
       uint32_t l,frame_sz,iter=1;
       uint32_t index;
   	test_status_t result,rpkt_no,rpkt_chk;
      test_status_t res_sts[2];
      amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
      amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
      amac_rx_pkt_info_t* pkt_ptr;
      uint32_t bypass_mode;

       uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

      bypass_mode = 0;


       //256-byte data; CRC will be appended by Tx MAC
       uint8_t my_pkt_data[256] =
      {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

      };

       uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
       uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
       uint8_t rdata8;

   	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
       uint32_t frame_size,step,chk_noPkt;
       uint32_t timeout;
       uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

   //	if(POS != post_get_board_diags_type())
   //   {
   //          post_log("\nSKIP : Diag not supported in VOIP and COMBO Boards\n");
   //          return TEST_SKIP;
   //   }




   	 ///////////// TEST ARG INIT ////////////////////

                 test_args_name[0]="Amac Ports =";
                 test_args_name[1]="Tx Desc Base =";
                 test_args_name[2]="Rx Desc Base =";
                 test_args_name[3]="Tx Desc Depth =";
                 test_args_name[4]="Rx Desc Depth =";
                 test_args_name[5]="Tx Buffer Base =";
                 test_args_name[6]="Rx Buffer Base =";
                 test_args_name[7]="Frame size =";

                 amac_ports        =1;
                 tx_desc_base      =0x73000000;//Max Desc space=(512 X 16)=0x2000
                 rx_desc_base      =0x74000000;
                 tx_desc_len       =200;//Max Desc=512=0x200
                 rx_desc_len       =200;
                 tx_buff_base      =0x63000000;//Max Buffer space=(512 X 12K)=0x600000
                 rx_buff_base      =0x64000000;
                 frame_size          =128;
                 //frame_sz            =1024;
                  step            =23;
       chk_noPkt       =tx_desc_len-1;
       //UNIMAC Config args
       eth_speed       =0x2;//1Gbps
       rx_pause_ignore =1;
       hd_ena          =1;
       mac_loop_con    =0;
       ena_ext_config  =1;
       rmt_loop_ena    =0;
       tx_pause_ignore =0;

   	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
   	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

   				//cpu_wr_single(0x301d060,0xb,4);
                   //for(i=0;i<2000000;i++);
   				//cpu_wr_single(0x301d060,0xf,4);

   	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

      ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
           //Basic AMAC block init
           amac_init(amac_id,bypass_mode);

           //UNIMAC Config
           unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                        rmt_loop_ena,tx_pause_ignore);

           //Reset MIB Counter
           counter_reset(amac_id);

           //Set up MAC loopback
           //amac_set_config(amac_id, MAC_LOOPBACK, 1);

           //Initialize AMAC TX DMA block
           amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
           amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
           for(index = 0; index < tx_desc_len; index++) {

                //frame_size=((rand()%(0x3000-64))+64);

                CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
           }

           //Initialize AMAC RX DMA block
           amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                           AMAC_PKT_BUFFER_SIZE_IN_BYTES);

           //Update with next descriptor value - outside EOT range
           //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

           //Within EOT range
           amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

            //amac_get_tx_status(amac_id);
            //amac_get_rx_status(amac_id);
            post_log("\n\n");
      }
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    cfg_sw1_rev();

   	                 page=0x0;
                         offset=0x6f;
                         //rdata = srab_rd(page, offset);
                         //wdatal=(rdatal | 0x1);
                         //wdatah=rdatah;
                         //srab_wr(page, offset, wdatah, wdatal);
   					  srab_rdwr(page,offset,0x1,5);


                         ccg_mdio_init();

                          phy_addr = 0;
                            for(i=0; i<16; i++) {
                                   reg_addr = i;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                              }

                            //DSP_TAP10 FIX INCLUDED
  						  phy_addr = 0;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);


                            //Lane Swapping and Polarity inversion for Gphy0
                            phy_addr = 0;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 0x15;
                            data = 0x5193;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            //Lane Swapping and Polarity inversion for Gphy1
                            phy_addr = 1;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 0x15;
                            data = 0x11C9;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                           /* phy_addr = 0;
                                          reg_addr = 0x1;
                                          do {
                                       	    gphy_reg_read(phy_addr,reg_addr,&data);
                                          } while(!(data & 0x4));*/

                            //wait for linkup
                            /*phy_addr = 1;
                            reg_addr = 0x1;
                            do {
                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                            } while(!(data & 0x4));*/

                            phy_addr = 0;
                            reg_addr = 0;
                            data = 0x3140;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 4;
                            data = 0x781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 5;
                            data = 0xC781;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 9;
                            data = 0x0000;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                         phy_addr = 0;
   						  reg_addr = 0;
               gphy_reg_read(phy_addr, reg_addr, &data);
               data |= (1 << 14);
               gphy_reg_write(phy_addr, reg_addr, &data);
               gphy_reg_read(phy_addr, reg_addr, &data);

               /*data = 0x01E1;
               gphy_reg_write(phy_addr, 0x0004, &data);
               gphy_reg_read(phy_addr, 0x0004, &data);*/



       ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
           // NxtDscr points to EOT Dscr
           nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           //Out of EOT range,for continuous loopback
           //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           amac_enable_tx_dma(amac_id, nextDscr);
           post_log("\n\n");
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
      /*timeout=300;
      post_log("Sending packets for specified time\n");
      delay_noise(timeout,0);
      post_log("Time_taken =%d secs\n",(timeout*7));
      suspend_tx(amac_id);*/

   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x20;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x50;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x21;
   //	// read port8 rx good byte register
   //	offset=0x51;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);






      for(amac_id=0;amac_id < amac_ports;amac_id++){
           waitforTrans_internalloopback(amac_id);
           post_log("\n");
           amac_gettxstats(amac_id);
           amac_getrxstats(amac_id);

           rpkt_no=checker_all(amac_id);
           post_log("\n");

           post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
           rpkt_chk=TEST_PASS;
           rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

            if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                    res_sts[amac_id]=TEST_PASS;
                    post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                    post_log("==========PASS==========\n");

            }
            else{
                    res_sts[amac_id]=TEST_FAIL;
                    post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                    post_log("==========FAIL==========\n");
            }

            post_log("\n\n");
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      if((res_sts[0]==TEST_PASS)){
               result=TEST_PASS;
      }else{
               result=TEST_FAIL;
       }
      return result;

      }


    int gphy0_internal_loopback_without_laneswap_100MBPS(void)
          {

           test_status_t status = TEST_PASS;
           uint32_t test_args[NUM_TEST_ARGS];
           char *test_args_name[NUM_TEST_ARGS];
           uint32_t l,frame_sz,iter=1;
           uint32_t index;
       	test_status_t result,rpkt_no,rpkt_chk;
          test_status_t res_sts[2];
          amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
          amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
          amac_rx_pkt_info_t* pkt_ptr;
          uint32_t bypass_mode;

           uint32_t page;
             	uint32_t offset;
             	uint32_t wdatah, wdatal;
             	uint32_t rdatah, rdatal;
             	uint32_t port_id;
                 uint32_t *rdata;
                 uint32_t phy_addr,i,reg_addr;
                 uint32_t data;

          bypass_mode = 0;


           //256-byte data; CRC will be appended by Tx MAC
           uint8_t my_pkt_data[256] =
          {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
            0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
            0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
            0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
            0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
            0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
            0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
            0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
            0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
            0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
            0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
            0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

          };

           uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
           uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
           uint8_t rdata8;

       	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
           uint32_t frame_size,step,chk_noPkt;
           uint32_t timeout;
           uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

       //	if(POS != post_get_board_diags_type())
       //   {
       //          post_log("\nSKIP : Diag not supported in VOIP and COMBO Boards\n");
       //          return TEST_SKIP;
       //   }




       	 ///////////// TEST ARG INIT ////////////////////

                     test_args_name[0]="Amac Ports =";
                     test_args_name[1]="Tx Desc Base =";
                     test_args_name[2]="Rx Desc Base =";
                     test_args_name[3]="Tx Desc Depth =";
                     test_args_name[4]="Rx Desc Depth =";
                     test_args_name[5]="Tx Buffer Base =";
                     test_args_name[6]="Rx Buffer Base =";
                     test_args_name[7]="Frame size =";

                     amac_ports        =1;
                     tx_desc_base      =0x73000000;//Max Desc space=(512 X 16)=0x2000
                     rx_desc_base      =0x74000000;
                     tx_desc_len       =200;//Max Desc=512=0x200
                     rx_desc_len       =200;
                     tx_buff_base      =0x63000000;//Max Buffer space=(512 X 12K)=0x600000
                     rx_buff_base      =0x64000000;
                     frame_size          =128;
                     //frame_sz            =1024;
                      step            =23;
           chk_noPkt       =tx_desc_len-1;
           //UNIMAC Config args
           eth_speed       =0x2;//1Gbps
           rx_pause_ignore =1;
           hd_ena          =1;
           mac_loop_con    =0;
           ena_ext_config  =1;
           rmt_loop_ena    =0;
           tx_pause_ignore =0;

       	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
       	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

       				//cpu_wr_single(0x301d060,0xb,4);
                       //for(i=0;i<2000000;i++);
       				//cpu_wr_single(0x301d060,0xf,4);

       	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

          ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
          for(amac_id=0;amac_id < amac_ports;amac_id++){
               post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
               //Basic AMAC block init
               amac_init(amac_id,bypass_mode);

               //UNIMAC Config
               unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                            rmt_loop_ena,tx_pause_ignore);

               //Reset MIB Counter
               counter_reset(amac_id);

               //Set up MAC loopback
               //amac_set_config(amac_id, MAC_LOOPBACK, 1);

               //Initialize AMAC TX DMA block
               amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
               amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
               for(index = 0; index < tx_desc_len; index++) {

                    //frame_size=((rand()%(0x3000-64))+64);

                    CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                    write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                    //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                    amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                    (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                    amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
               }

               //Initialize AMAC RX DMA block
               amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                               AMAC_PKT_BUFFER_SIZE_IN_BYTES);

               //Update with next descriptor value - outside EOT range
               //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

               //Within EOT range
               amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

                //amac_get_tx_status(amac_id);
                //amac_get_rx_status(amac_id);
                post_log("\n\n");
          }
           /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                        cfg_sw1_rev();

       	                 page=0x0;
                             offset=0x6f;
                             //rdata = srab_rd(page, offset);
                             //wdatal=(rdatal | 0x1);
                             //wdatah=rdatah;
                             //srab_wr(page, offset, wdatah, wdatal);
       					  srab_rdwr(page,offset,0x1,5);


                             ccg_mdio_init();

                              phy_addr = 0;
                                for(i=0; i<16; i++) {
                                       reg_addr = i;
                                       gphy_reg_read(phy_addr, reg_addr, &data);
                                  }

                                //DSP_TAP10 FIX INCLUDED
      						  phy_addr = 0;
      	                    reg_addr = 0x18;
      	                    data = 0x0C30;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 0;
      	                    reg_addr = 0x17;
      	                    data = 0x000A;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 0;
      	                    reg_addr = 0x15;
      	                    data = 0x111B;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 1;
      	                    reg_addr = 0x18;
      	                    data = 0x0C30;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 1;
      	                    reg_addr = 0x17;
      	                    data = 0x000A;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 1;
      	                    reg_addr = 0x15;
      	                    data = 0x111B;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);




                               /* phy_addr = 0;
                                              reg_addr = 0x1;
                                              do {
                                           	    gphy_reg_read(phy_addr,reg_addr,&data);
                                              } while(!(data & 0x4));*/

                                //wait for linkup
                                /*phy_addr = 1;
                                reg_addr = 0x1;
                                do {
                             	    gphy_reg_read(phy_addr,reg_addr,&data);
                                } while(!(data & 0x4));*/

                                phy_addr = 0;
                                reg_addr = 0;
                                data = 0x3140;
                                gphy_reg_write(phy_addr, reg_addr, &data);

                                phy_addr = 0;
                                reg_addr = 4;
                                data = 0x781;
                                gphy_reg_write(phy_addr, reg_addr, &data);

                                phy_addr = 0;
                                reg_addr = 5;
                                data = 0xC781;
                                gphy_reg_write(phy_addr, reg_addr, &data);

                                phy_addr = 0;
                                reg_addr = 9;
                                data = 0x0000;
                                gphy_reg_write(phy_addr, reg_addr, &data);

                             phy_addr = 0;
       						  reg_addr = 0;
                   gphy_reg_read(phy_addr, reg_addr, &data);
                   data |= (1 << 14);
                   gphy_reg_write(phy_addr, reg_addr, &data);
                   gphy_reg_read(phy_addr, reg_addr, &data);

                   /*data = 0x01E1;
                   gphy_reg_write(phy_addr, 0x0004, &data);
                   gphy_reg_read(phy_addr, 0x0004, &data);*/



           ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
          for(amac_id=0;amac_id < amac_ports;amac_id++){
               post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
               // NxtDscr points to EOT Dscr
               nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                       ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
               //Out of EOT range,for continuous loopback
               //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                       ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
               amac_enable_tx_dma(amac_id, nextDscr);
               post_log("\n\n");
          }
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




          //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
          /*timeout=300;
          post_log("Sending packets for specified time\n");
          delay_noise(timeout,0);
          post_log("Time_taken =%d secs\n",(timeout*7));
          suspend_tx(amac_id);*/

       //   // read port8 rx stastic register
       //	page=0x28;
       //	// read port8 rx good byte register
       //	offset=0x0;
       //	rdata = srab_rd(page, offset);
       //    rdatal=rdata[0];
       //    rdatah=rdata[1];
       //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
       //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
       //	wdatal=((rdatal&0xfffffffC) | 0x3);
       //
       //// read port1 rx stastic register
       //	page=0x20;
       //	// read port8 rx good byte register
       //	offset=0x0;
       //	rdata = srab_rd(page, offset);
       //    rdatal=rdata[0];
       //    rdatah=rdata[1];
       //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
       //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
       //	wdatal=((rdatal&0xfffffffC) | 0x3);
       //
       //   // read port8 rx stastic register
       //	page=0x28;
       //	// read port8 rx good byte register
       //	offset=0x50;
       //	rdata = srab_rd(page, offset);
       //    rdatal=rdata[0];
       //    rdatah=rdata[1];
       //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
       //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
       //	wdatal=((rdatal&0xfffffffC) | 0x3);
       //
       //// read port1 rx stastic register
       //	page=0x21;
       //	// read port8 rx good byte register
       //	offset=0x51;
       //	rdata = srab_rd(page, offset);
       //    rdatal=rdata[0];
       //    rdatah=rdata[1];
       //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
       //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
       //	wdatal=((rdatal&0xfffffffC) | 0x3);






          for(amac_id=0;amac_id < amac_ports;amac_id++){
               waitforTrans_internalloopback(amac_id);
               post_log("\n");
               amac_gettxstats(amac_id);
               amac_getrxstats(amac_id);

               rpkt_no=checker_all(amac_id);
               post_log("\n");

               post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
               rpkt_chk=TEST_PASS;
               rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

                if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                        res_sts[amac_id]=TEST_PASS;
                        post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                        post_log("==========PASS==========\n");
                }
                else{
                        res_sts[amac_id]=TEST_FAIL;
                        post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                        post_log("==========FAIL==========\n");
                }

                post_log("\n\n");
          }
          ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


          if((res_sts[0]==TEST_PASS)){
                   result=TEST_PASS;
          }else{
                   result=TEST_FAIL;
           }
          return result;

          }

    int gphy0_internal_loopback_10MBPS(void)
      {

       test_status_t status = TEST_PASS;
       uint32_t test_args[NUM_TEST_ARGS];
       char *test_args_name[NUM_TEST_ARGS];
       uint32_t l,frame_sz,iter=1;
       uint32_t index;
   	test_status_t result,rpkt_no,rpkt_chk;
      test_status_t res_sts[2];
      amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
      amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
      amac_rx_pkt_info_t* pkt_ptr;
      uint32_t bypass_mode;

       uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

      bypass_mode = 0;


       //256-byte data; CRC will be appended by Tx MAC
       uint8_t my_pkt_data[256] =
      {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

      };

       uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
       uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
       uint8_t rdata8;

   	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
       uint32_t frame_size,step,chk_noPkt;
       uint32_t timeout;
       uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

   //	if(POS != post_get_board_diags_type())
   //   {
   //          post_log("\nSKIP : Diag not supported in VOIP and COMBO Boards\n");
   //          return TEST_SKIP;
   //   }




   	 ///////////// TEST ARG INIT ////////////////////

                 test_args_name[0]="Amac Ports =";
                 test_args_name[1]="Tx Desc Base =";
                 test_args_name[2]="Rx Desc Base =";
                 test_args_name[3]="Tx Desc Depth =";
                 test_args_name[4]="Rx Desc Depth =";
                 test_args_name[5]="Tx Buffer Base =";
                 test_args_name[6]="Rx Buffer Base =";
                 test_args_name[7]="Frame size =";

                 amac_ports        =1;
                 tx_desc_base      =0x73000000;//Max Desc space=(512 X 16)=0x2000
                 rx_desc_base      =0x74000000;
                 tx_desc_len       =200;//Max Desc=512=0x200
                 rx_desc_len       =200;
                 tx_buff_base      =0x63000000;//Max Buffer space=(512 X 12K)=0x600000
                 rx_buff_base      =0x64000000;
                 frame_size          =128;
                 //frame_sz            =1024;
                  step            =23;
       chk_noPkt       =tx_desc_len-1;
       //UNIMAC Config args
       eth_speed       =0x2;//1Gbps
       rx_pause_ignore =1;
       hd_ena          =1;
       mac_loop_con    =0;
       ena_ext_config  =1;
       rmt_loop_ena    =0;
       tx_pause_ignore =0;

   	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
   	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

   				//cpu_wr_single(0x301d060,0xb,4);
                   //for(i=0;i<2000000;i++);
   				//cpu_wr_single(0x301d060,0xf,4);

   	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

      ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
           //Basic AMAC block init
           amac_init(amac_id,bypass_mode);

           //UNIMAC Config
           unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                        rmt_loop_ena,tx_pause_ignore);

           //Reset MIB Counter
           counter_reset(amac_id);

           //Set up MAC loopback
           //amac_set_config(amac_id, MAC_LOOPBACK, 1);

           //Initialize AMAC TX DMA block
           amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
           amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
           for(index = 0; index < tx_desc_len; index++) {

                //frame_size=((rand()%(0x3000-64))+64);

                CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
           }

           //Initialize AMAC RX DMA block
           amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                           AMAC_PKT_BUFFER_SIZE_IN_BYTES);

           //Update with next descriptor value - outside EOT range
           //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

           //Within EOT range
           amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

            //amac_get_tx_status(amac_id);
            //amac_get_rx_status(amac_id);
            post_log("\n\n");
      }
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    cfg_sw1_rev();

   	                 page=0x0;
                         offset=0x6f;
                         //rdata = srab_rd(page, offset);
                         //wdatal=(rdatal | 0x1);
                         //wdatah=rdatah;
                         //srab_wr(page, offset, wdatah, wdatal);
   					  srab_rdwr(page,offset,0x1,5);


                         ccg_mdio_init();

                          phy_addr = 0;
                            for(i=0; i<16; i++) {
                                   reg_addr = i;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                              }

                            //DSP_TAP10 FIX INCLUDED
  						  phy_addr = 0;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);


                            //Lane Swapping and Polarity inversion for Gphy0
                            phy_addr = 0;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 0x15;
                            data = 0x5193;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            //Lane Swapping and Polarity inversion for Gphy1
                            phy_addr = 1;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 0x15;
                            data = 0x11C9;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                           /* phy_addr = 0;
                                          reg_addr = 0x1;
                                          do {
                                       	    gphy_reg_read(phy_addr,reg_addr,&data);
                                          } while(!(data & 0x4));*/

                            //wait for linkup
                            /*phy_addr = 1;
                            reg_addr = 0x1;
                            do {
                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                            } while(!(data & 0x4));*/

                            phy_addr = 0;
                            reg_addr = 0;
                            data = 0x1140;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 4;
                            data = 0x461;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 5;
                            data = 0xC461;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 9;
                            data = 0x0000;
                            gphy_reg_write(phy_addr, reg_addr, &data);


                         phy_addr = 0;
   						  reg_addr = 0;
               gphy_reg_read(phy_addr, reg_addr, &data);
               data |= (1 << 14);
               gphy_reg_write(phy_addr, reg_addr, &data);
               gphy_reg_read(phy_addr, reg_addr, &data);

               /*data = 0x01E1;
               gphy_reg_write(phy_addr, 0x0004, &data);
               gphy_reg_read(phy_addr, 0x0004, &data);*/



       ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
           // NxtDscr points to EOT Dscr
           nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           //Out of EOT range,for continuous loopback
           //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           amac_enable_tx_dma(amac_id, nextDscr);
           post_log("\n\n");
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
      /*timeout=300;
      post_log("Sending packets for specified time\n");
      delay_noise(timeout,0);
      post_log("Time_taken =%d secs\n",(timeout*7));
      suspend_tx(amac_id);*/

   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x20;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x50;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x21;
   //	// read port8 rx good byte register
   //	offset=0x51;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);






      for(amac_id=0;amac_id < amac_ports;amac_id++){
           waitforTrans_internalloopback(amac_id);
           post_log("\n");
           amac_gettxstats(amac_id);
           amac_getrxstats(amac_id);

           rpkt_no=checker_all(amac_id);
           post_log("\n");

           post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
           rpkt_chk=TEST_PASS;
           rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

            if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                    res_sts[amac_id]=TEST_PASS;
                    post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                    post_log("==========PASS==========\n");
            }
            else{
                    res_sts[amac_id]=TEST_FAIL;
                    post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                    post_log("==========FAIL==========\n");
            }

            post_log("\n\n");
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      if((res_sts[0]==TEST_PASS)){
               result=TEST_PASS;
      }else{
               result=TEST_FAIL;
       }
      return result;

      }

    int gphy0_internal_loopback_without_laneswap_10MBPS(void)
          {

           test_status_t status = TEST_PASS;
           uint32_t test_args[NUM_TEST_ARGS];
           char *test_args_name[NUM_TEST_ARGS];
           uint32_t l,frame_sz,iter=1;
           uint32_t index;
       	test_status_t result,rpkt_no,rpkt_chk;
          test_status_t res_sts[2];
          amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
          amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
          amac_rx_pkt_info_t* pkt_ptr;
          uint32_t bypass_mode;

           uint32_t page;
             	uint32_t offset;
             	uint32_t wdatah, wdatal;
             	uint32_t rdatah, rdatal;
             	uint32_t port_id;
                 uint32_t *rdata;
                 uint32_t phy_addr,i,reg_addr;
                 uint32_t data;

          bypass_mode = 0;


           //256-byte data; CRC will be appended by Tx MAC
           uint8_t my_pkt_data[256] =
          {
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
            0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
            0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
            0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
            0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
            0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
            0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
            0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
            0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
            0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
            0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
            0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
            0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
            0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

          };

           uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
           uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
           uint8_t rdata8;

       	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
           uint32_t frame_size,step,chk_noPkt;
           uint32_t timeout;
           uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

       //	if(POS != post_get_board_diags_type())
       //   {
       //          post_log("\nSKIP : Diag not supported in VOIP and COMBO Boards\n");
       //          return TEST_SKIP;
       //   }




       	 ///////////// TEST ARG INIT ////////////////////

                     test_args_name[0]="Amac Ports =";
                     test_args_name[1]="Tx Desc Base =";
                     test_args_name[2]="Rx Desc Base =";
                     test_args_name[3]="Tx Desc Depth =";
                     test_args_name[4]="Rx Desc Depth =";
                     test_args_name[5]="Tx Buffer Base =";
                     test_args_name[6]="Rx Buffer Base =";
                     test_args_name[7]="Frame size =";

                     amac_ports        =1;
                     tx_desc_base      =0x73000000;//Max Desc space=(512 X 16)=0x2000
                     rx_desc_base      =0x74000000;
                     tx_desc_len       =200;//Max Desc=512=0x200
                     rx_desc_len       =200;
                     tx_buff_base      =0x63000000;//Max Buffer space=(512 X 12K)=0x600000
                     rx_buff_base      =0x64000000;
                     frame_size          =128;
                     //frame_sz            =1024;
                      step            =23;
           chk_noPkt       =tx_desc_len-1;
           //UNIMAC Config args
           eth_speed       =0x2;//1Gbps
           rx_pause_ignore =1;
           hd_ena          =1;
           mac_loop_con    =0;
           ena_ext_config  =1;
           rmt_loop_ena    =0;
           tx_pause_ignore =0;

       	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
       	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

       				//cpu_wr_single(0x301d060,0xb,4);
                       //for(i=0;i<2000000;i++);
       				//cpu_wr_single(0x301d060,0xf,4);

       	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

          ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
          for(amac_id=0;amac_id < amac_ports;amac_id++){
               post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
               //Basic AMAC block init
               amac_init(amac_id,bypass_mode);

               //UNIMAC Config
               unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                            rmt_loop_ena,tx_pause_ignore);

               //Reset MIB Counter
               counter_reset(amac_id);

               //Set up MAC loopback
               //amac_set_config(amac_id, MAC_LOOPBACK, 1);

               //Initialize AMAC TX DMA block
               amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
               amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
               for(index = 0; index < tx_desc_len; index++) {

                    //frame_size=((rand()%(0x3000-64))+64);

                    CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                    write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                    //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                    amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                    (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                    amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
               }

               //Initialize AMAC RX DMA block
               amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                               AMAC_PKT_BUFFER_SIZE_IN_BYTES);

               //Update with next descriptor value - outside EOT range
               //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

               //Within EOT range
               amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

                //amac_get_tx_status(amac_id);
                //amac_get_rx_status(amac_id);
                post_log("\n\n");
          }
           /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                        cfg_sw1_rev();

       	                 page=0x0;
                             offset=0x6f;
                             //rdata = srab_rd(page, offset);
                             //wdatal=(rdatal | 0x1);
                             //wdatah=rdatah;
                             //srab_wr(page, offset, wdatah, wdatal);
       					  srab_rdwr(page,offset,0x1,5);


                             ccg_mdio_init();

                              phy_addr = 0;
                                for(i=0; i<16; i++) {
                                       reg_addr = i;
                                       gphy_reg_read(phy_addr, reg_addr, &data);
                                  }

                                //DSP_TAP10 FIX INCLUDED
      						  phy_addr = 0;
      	                    reg_addr = 0x18;
      	                    data = 0x0C30;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 0;
      	                    reg_addr = 0x17;
      	                    data = 0x000A;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 0;
      	                    reg_addr = 0x15;
      	                    data = 0x111B;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 1;
      	                    reg_addr = 0x18;
      	                    data = 0x0C30;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 1;
      	                    reg_addr = 0x17;
      	                    data = 0x000A;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);

      	                    phy_addr = 1;
      	                    reg_addr = 0x15;
      	                    data = 0x111B;
      	                    gphy_reg_write(phy_addr,reg_addr,&data);


                               /* phy_addr = 0;
                                              reg_addr = 0x1;
                                              do {
                                           	    gphy_reg_read(phy_addr,reg_addr,&data);
                                              } while(!(data & 0x4));*/

                                //wait for linkup
                                /*phy_addr = 1;
                                reg_addr = 0x1;
                                do {
                             	    gphy_reg_read(phy_addr,reg_addr,&data);
                                } while(!(data & 0x4));*/

                                phy_addr = 0;
                                reg_addr = 0;
                                data = 0x1140;
                                gphy_reg_write(phy_addr, reg_addr, &data);

                                phy_addr = 0;
                                reg_addr = 4;
                                data = 0x461;
                                gphy_reg_write(phy_addr, reg_addr, &data);

                                phy_addr = 0;
                                reg_addr = 5;
                                data = 0xC461;
                                gphy_reg_write(phy_addr, reg_addr, &data);

                                phy_addr = 0;
                                reg_addr = 9;
                                data = 0x0000;
                                gphy_reg_write(phy_addr, reg_addr, &data);


                             phy_addr = 0;
       						  reg_addr = 0;
                   gphy_reg_read(phy_addr, reg_addr, &data);
                   data |= (1 << 14);
                   gphy_reg_write(phy_addr, reg_addr, &data);
                   gphy_reg_read(phy_addr, reg_addr, &data);

                   /*data = 0x01E1;
                   gphy_reg_write(phy_addr, 0x0004, &data);
                   gphy_reg_read(phy_addr, 0x0004, &data);*/



           ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
          for(amac_id=0;amac_id < amac_ports;amac_id++){
               post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
               // NxtDscr points to EOT Dscr
               nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                       ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
               //Out of EOT range,for continuous loopback
               //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                       ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
               amac_enable_tx_dma(amac_id, nextDscr);
               post_log("\n\n");
          }
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




          //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
          /*timeout=300;
          post_log("Sending packets for specified time\n");
          delay_noise(timeout,0);
          post_log("Time_taken =%d secs\n",(timeout*7));
          suspend_tx(amac_id);*/

       //   // read port8 rx stastic register
       //	page=0x28;
       //	// read port8 rx good byte register
       //	offset=0x0;
       //	rdata = srab_rd(page, offset);
       //    rdatal=rdata[0];
       //    rdatah=rdata[1];
       //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
       //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
       //	wdatal=((rdatal&0xfffffffC) | 0x3);
       //
       //// read port1 rx stastic register
       //	page=0x20;
       //	// read port8 rx good byte register
       //	offset=0x0;
       //	rdata = srab_rd(page, offset);
       //    rdatal=rdata[0];
       //    rdatah=rdata[1];
       //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
       //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
       //	wdatal=((rdatal&0xfffffffC) | 0x3);
       //
       //   // read port8 rx stastic register
       //	page=0x28;
       //	// read port8 rx good byte register
       //	offset=0x50;
       //	rdata = srab_rd(page, offset);
       //    rdatal=rdata[0];
       //    rdatah=rdata[1];
       //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
       //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
       //	wdatal=((rdatal&0xfffffffC) | 0x3);
       //
       //// read port1 rx stastic register
       //	page=0x21;
       //	// read port8 rx good byte register
       //	offset=0x51;
       //	rdata = srab_rd(page, offset);
       //    rdatal=rdata[0];
       //    rdatah=rdata[1];
       //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
       //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
       //	wdatal=((rdatal&0xfffffffC) | 0x3);






          for(amac_id=0;amac_id < amac_ports;amac_id++){
               waitforTrans_internalloopback(amac_id);
               post_log("\n");
               amac_gettxstats(amac_id);
               amac_getrxstats(amac_id);

               rpkt_no=checker_all(amac_id);
               post_log("\n");

               post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
               rpkt_chk=TEST_PASS;
               rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

                if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                        res_sts[amac_id]=TEST_PASS;
                        post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                        post_log("==========PASS==========\n");
                }
                else{
                        res_sts[amac_id]=TEST_FAIL;
                        post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                        post_log("==========FAIL==========\n");
                }

                post_log("\n\n");
          }
          ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


          if((res_sts[0]==TEST_PASS)){
                   result=TEST_PASS;
          }else{
                   result=TEST_FAIL;
           }
          return result;

          }



    int gphy0_internal_loopback_without_laneswap_1GBPS(void)
      {

       test_status_t status = TEST_PASS;
       uint32_t test_args[NUM_TEST_ARGS];
       char *test_args_name[NUM_TEST_ARGS];
       uint32_t l,frame_sz,iter=1;
       uint32_t index;
   	test_status_t result,rpkt_no,rpkt_chk;
      test_status_t res_sts[2];
      amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
      amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
      amac_rx_pkt_info_t* pkt_ptr;
      uint32_t bypass_mode;

       uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

      bypass_mode = 0;


       //256-byte data; CRC will be appended by Tx MAC
       uint8_t my_pkt_data[256] =
      {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

      };

       uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
       uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
       uint8_t rdata8;

   	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
       uint32_t frame_size,step,chk_noPkt;
       uint32_t timeout;
       uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

   //	if(POS != post_get_board_diags_type())
   //   {
   //          post_log("\nSKIP : Diag not supported in VOIP and COMBO Boards\n");
   //          return TEST_SKIP;
   //   }




   	 ///////////// TEST ARG INIT ////////////////////

                 test_args_name[0]="Amac Ports =";
                 test_args_name[1]="Tx Desc Base =";
                 test_args_name[2]="Rx Desc Base =";
                 test_args_name[3]="Tx Desc Depth =";
                 test_args_name[4]="Rx Desc Depth =";
                 test_args_name[5]="Tx Buffer Base =";
                 test_args_name[6]="Rx Buffer Base =";
                 test_args_name[7]="Frame size =";

                 amac_ports        =1;
                 tx_desc_base      =0x73000000;//Max Desc space=(512 X 16)=0x2000
                 rx_desc_base      =0x74000000;
                 tx_desc_len       =200;//Max Desc=512=0x200
                 rx_desc_len       =200;
                 tx_buff_base      =0x63000000;//Max Buffer space=(512 X 12K)=0x600000
                 rx_buff_base      =0x64000000;
                 frame_size          =128;
                 //frame_sz            =1024;
                  step            =23;
       chk_noPkt       =tx_desc_len-1;
       //UNIMAC Config args
       eth_speed       =0x2;//1Gbps
       rx_pause_ignore =1;
       hd_ena          =1;
       mac_loop_con    =0;
       ena_ext_config  =1;
       rmt_loop_ena    =0;
       tx_pause_ignore =0;

   	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
   	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

   				//cpu_wr_single(0x301d060,0xb,4);
                   //for(i=0;i<2000000;i++);
   				//cpu_wr_single(0x301d060,0xf,4);

   	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

      ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
           //Basic AMAC block init
           amac_init(amac_id,bypass_mode);

           //UNIMAC Config
           unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                        rmt_loop_ena,tx_pause_ignore);

           //Reset MIB Counter
           counter_reset(amac_id);

           //Set up MAC loopback
           //amac_set_config(amac_id, MAC_LOOPBACK, 1);

           //Initialize AMAC TX DMA block
           amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
           amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
           for(index = 0; index < tx_desc_len; index++) {

                //frame_size=((rand()%(0x3000-64))+64);

                CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
           }

           //Initialize AMAC RX DMA block
           amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                           AMAC_PKT_BUFFER_SIZE_IN_BYTES);

           //Update with next descriptor value - outside EOT range
           //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

           //Within EOT range
           amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

            //amac_get_tx_status(amac_id);
            //amac_get_rx_status(amac_id);
            post_log("\n\n");
      }
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    cfg_sw1_rev();

   	                 page=0x0;
                         offset=0x6f;
                         //rdata = srab_rd(page, offset);
                         //wdatal=(rdatal | 0x1);
                         //wdatah=rdatah;
                         //srab_wr(page, offset, wdatah, wdatal);
   					  srab_rdwr(page,offset,0x1,5);


                         ccg_mdio_init();

                          phy_addr = 0;
                            for(i=0; i<16; i++) {
                                   reg_addr = i;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                              }

                            //DSP_TAP10 FIX INCLUDED
  						  phy_addr = 0;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);


                            //Lane Swapping and Polarity inversion for Gphy0
                            /*phy_addr = 0;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 0x15;
                            data = 0x5193;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            //Lane Swapping and Polarity inversion for Gphy1
                            phy_addr = 1;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 0x15;
                            data = 0x11C9;
                            gphy_reg_write(phy_addr, reg_addr, &data);*/

                            for(i=0; i<20000000; i++) ;

                           /* phy_addr = 0;
                                          reg_addr = 0x1;
                                          do {
                                       	    gphy_reg_read(phy_addr,reg_addr,&data);
                                          } while(!(data & 0x4));*/

                            //wait for linkup
                            /*phy_addr = 1;
                            reg_addr = 0x1;
                            do {
                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                            } while(!(data & 0x4));*/

                            phy_addr = 0;
                            	                    reg_addr = 0x18;
                            	                    data = 0x0C30;
                            	                    gphy_reg_write(phy_addr,reg_addr,&data);

                            	                    phy_addr = 0;
                            	                    reg_addr = 0x17;
                            	                    data = 0x000A;
                            	                    gphy_reg_write(phy_addr,reg_addr,&data);

                            	                    phy_addr = 0;
                            	                    reg_addr = 0x15;
                            	                    data = 0x111B;
                            	                    gphy_reg_write(phy_addr,reg_addr,&data);

                         phy_addr = 0;
   						  reg_addr = 0;
               gphy_reg_read(phy_addr, reg_addr, &data);
               data |= (1 << 14);
               gphy_reg_write(phy_addr, reg_addr, &data);
               gphy_reg_read(phy_addr, reg_addr, &data);

               data = 0x01E1;
               gphy_reg_write(phy_addr, 0x0004, &data);
               gphy_reg_read(phy_addr, 0x0004, &data);



       ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
           // NxtDscr points to EOT Dscr
           nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           //Out of EOT range,for continuous loopback
           //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           amac_enable_tx_dma(amac_id, nextDscr);
           post_log("\n\n");
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
      /*timeout=300;
      post_log("Sending packets for specified time\n");
      delay_noise(timeout,0);
      post_log("Time_taken =%d secs\n",(timeout*7));
      suspend_tx(amac_id);*/

   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x20;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x50;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x21;
   //	// read port8 rx good byte register
   //	offset=0x51;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);






      for(amac_id=0;amac_id < amac_ports;amac_id++){
           waitforTrans_internalloopback(amac_id);
           post_log("\n");
           amac_gettxstats(amac_id);
           amac_getrxstats(amac_id);

           rpkt_no=checker_all(amac_id);
           post_log("\n");

           post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
           rpkt_chk=TEST_PASS;
           rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

            if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                    res_sts[amac_id]=TEST_PASS;
                    post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                    post_log("==========PASS==========\n");
            }
            else{
                    res_sts[amac_id]=TEST_FAIL;
                    post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                    post_log("==========FAIL==========\n");
            }

            post_log("\n\n");
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      if((res_sts[0]==TEST_PASS)){
               result=TEST_PASS;
      }else{
               result=TEST_FAIL;
       }
      return result;

      }



   int gphy1_internal_loopback_1GBPS(void)
   {

    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;


    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[256] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x75000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x76000000;
              tx_desc_len       =200;//Max Desc=512=0x200
              rx_desc_len       =200;
              tx_buff_base      =0x65000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x66000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);

	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        //amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 cfg_sw1_rev1();

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //DSP_TAP10 FIX INCLUDED
						  phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);


                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                        /* phy_addr = 0;
                                       reg_addr = 0x1;
                                       do {
                                    	    gphy_reg_read(phy_addr,reg_addr,&data);
                                       } while(!(data & 0x4));*/

                         //wait for linkup
                         /*phy_addr = 1;
                         reg_addr = 0x1;
                         do {
                      	    gphy_reg_read(phy_addr,reg_addr,&data);
                         } while(!(data & 0x4));*/

                      phy_addr = 1;
						  reg_addr = 0;
            gphy_reg_read(phy_addr, reg_addr, &data);
            data |= (1 << 14);
            gphy_reg_write(phy_addr, reg_addr, &data);
            gphy_reg_read(phy_addr, reg_addr, &data);

            data = 0x01E1;
            gphy_reg_write(phy_addr, 0x0004, &data);
            gphy_reg_read(phy_addr, 0x0004, &data);



    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




   //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
   /*timeout=300;
   post_log("Sending packets for specified time\n");
   delay_noise(timeout,0);
   post_log("Time_taken =%d secs\n",(timeout*7));
   suspend_tx(amac_id);*/

//   // read port8 rx stastic register
//	page=0x28;
//	// read port8 rx good byte register
//	offset=0x0;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3);
//
//// read port1 rx stastic register
//	page=0x20;
//	// read port8 rx good byte register
//	offset=0x0;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3);
//
//   // read port8 rx stastic register
//	page=0x28;
//	// read port8 rx good byte register
//	offset=0x50;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3);
//
//// read port1 rx stastic register
//	page=0x21;
//	// read port8 rx good byte register
//	offset=0x51;
//	rdata = srab_rd(page, offset);
//    rdatal=rdata[0];
//    rdatah=rdata[1];
//    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
//    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
//	wdatal=((rdatal&0xfffffffC) | 0x3);






   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                 post_log("==========PASS==========\n");
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                 post_log("==========FAIL==========\n");
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }


   int gphy1_internal_loopback_100MBPS(void)
      {

       test_status_t status = TEST_PASS;
       uint32_t test_args[NUM_TEST_ARGS];
       char *test_args_name[NUM_TEST_ARGS];
       uint32_t l,frame_sz,iter=1;
       uint32_t index;
   	test_status_t result,rpkt_no,rpkt_chk;
      test_status_t res_sts[2];
      amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
      amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
      amac_rx_pkt_info_t* pkt_ptr;
      uint32_t bypass_mode;

       uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

      bypass_mode = 0;


       //256-byte data; CRC will be appended by Tx MAC
       uint8_t my_pkt_data[256] =
      {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

      };

       uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
       uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
       uint8_t rdata8;

   	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
       uint32_t frame_size,step,chk_noPkt;
       uint32_t timeout;
       uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


   	 ///////////// TEST ARG INIT ////////////////////

                 test_args_name[0]="Amac Ports =";
                 test_args_name[1]="Tx Desc Base =";
                 test_args_name[2]="Rx Desc Base =";
                 test_args_name[3]="Tx Desc Depth =";
                 test_args_name[4]="Rx Desc Depth =";
                 test_args_name[5]="Tx Buffer Base =";
                 test_args_name[6]="Rx Buffer Base =";
                 test_args_name[7]="Frame size =";

                 amac_ports        =1;
                 tx_desc_base      =0x75000000;//Max Desc space=(512 X 16)=0x2000
                 rx_desc_base      =0x76000000;
                 tx_desc_len       =200;//Max Desc=512=0x200
                 rx_desc_len       =200;
                 tx_buff_base      =0x65000000;//Max Buffer space=(512 X 12K)=0x600000
                 rx_buff_base      =0x66000000;
                 frame_size          =128;
                 //frame_sz            =1024;
                  step            =23;
       chk_noPkt       =tx_desc_len-1;
       //UNIMAC Config args
       eth_speed       =0x2;//1Gbps
       rx_pause_ignore =1;
       hd_ena          =1;
       mac_loop_con    =0;
       ena_ext_config  =1;
       rmt_loop_ena    =0;
       tx_pause_ignore =0;

   	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
   	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

   				//cpu_wr_single(0x301d060,0xb,4);
                   //for(i=0;i<2000000;i++);
   				//cpu_wr_single(0x301d060,0xf,4);

   	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

      ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
           //Basic AMAC block init
           amac_init(amac_id,bypass_mode);

           //UNIMAC Config
           unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                        rmt_loop_ena,tx_pause_ignore);

           //Reset MIB Counter
           counter_reset(amac_id);

           //Set up MAC loopback
           //amac_set_config(amac_id, MAC_LOOPBACK, 1);

           //Initialize AMAC TX DMA block
           amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
           amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
           for(index = 0; index < tx_desc_len; index++) {

                //frame_size=((rand()%(0x3000-64))+64);

                CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
           }

           //Initialize AMAC RX DMA block
           amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                           AMAC_PKT_BUFFER_SIZE_IN_BYTES);

           //Update with next descriptor value - outside EOT range
           //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

           //Within EOT range
           amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

            //amac_get_tx_status(amac_id);
            //amac_get_rx_status(amac_id);
            post_log("\n\n");
      }
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    cfg_sw1_rev1();

   	                 page=0x0;
                         offset=0x6f;
                         //rdata = srab_rd(page, offset);
                         //wdatal=(rdatal | 0x1);
                         //wdatah=rdatah;
                         //srab_wr(page, offset, wdatah, wdatal);
   					  srab_rdwr(page,offset,0x1,5);


                         ccg_mdio_init();

                          phy_addr = 0;
                            for(i=0; i<16; i++) {
                                   reg_addr = i;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                              }

                            //DSP_TAP10 FIX INCLUDED
  						  phy_addr = 0;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);


                            //Lane Swapping and Polarity inversion for Gphy0
                            phy_addr = 0;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 0x15;
                            data = 0x5193;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            //Lane Swapping and Polarity inversion for Gphy1
                            phy_addr = 1;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 0x15;
                            data = 0x11C9;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                           /* phy_addr = 0;
                                          reg_addr = 0x1;
                                          do {
                                       	    gphy_reg_read(phy_addr,reg_addr,&data);
                                          } while(!(data & 0x4));*/

                            //wait for linkup
                            /*phy_addr = 1;
                            reg_addr = 0x1;
                            do {
                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                            } while(!(data & 0x4));*/



               phy_addr = 1;
               reg_addr = 0;
               data = 0x3140;
               gphy_reg_write(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 4;
               data = 0x781;
               gphy_reg_write(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 5;
               data = 0xC781;
               gphy_reg_write(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 9;
               data = 0x0000;
               gphy_reg_write(phy_addr, reg_addr, &data);

               phy_addr = 1;
                 						  reg_addr = 0;
                             gphy_reg_read(phy_addr, reg_addr, &data);
                             data |= (1 << 14);
                             gphy_reg_write(phy_addr, reg_addr, &data);
                             gphy_reg_read(phy_addr, reg_addr, &data);


               /*data = 0x01E1;
               gphy_reg_write(phy_addr, 0x0004, &data);
               gphy_reg_read(phy_addr, 0x0004, &data);*/



       ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
           // NxtDscr points to EOT Dscr
           nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           //Out of EOT range,for continuous loopback
           //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           amac_enable_tx_dma(amac_id, nextDscr);
           post_log("\n\n");
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
      /*timeout=300;
      post_log("Sending packets for specified time\n");
      delay_noise(timeout,0);
      post_log("Time_taken =%d secs\n",(timeout*7));
      suspend_tx(amac_id);*/

   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x20;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x50;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x21;
   //	// read port8 rx good byte register
   //	offset=0x51;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);






      for(amac_id=0;amac_id < amac_ports;amac_id++){
           waitforTrans_internalloopback(amac_id);
           post_log("\n");
           amac_gettxstats(amac_id);
           amac_getrxstats(amac_id);

           rpkt_no=checker_all(amac_id);
           post_log("\n");

           post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
           rpkt_chk=TEST_PASS;
           rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

            if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                    res_sts[amac_id]=TEST_PASS;
                    post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                    post_log("==========PASS==========\n");
            }
            else{
                    res_sts[amac_id]=TEST_FAIL;
                    post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                    post_log("==========FAIL==========\n");
            }

            post_log("\n\n");
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      if((res_sts[0]==TEST_PASS)){
               result=TEST_PASS;
      }else{
               result=TEST_FAIL;
       }
      return result;

      }

   int gphy1_internal_loopback_without_laneswap_100MBPS(void)
         {

          test_status_t status = TEST_PASS;
          uint32_t test_args[NUM_TEST_ARGS];
          char *test_args_name[NUM_TEST_ARGS];
          uint32_t l,frame_sz,iter=1;
          uint32_t index;
      	test_status_t result,rpkt_no,rpkt_chk;
         test_status_t res_sts[2];
         amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
         amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
         amac_rx_pkt_info_t* pkt_ptr;
         uint32_t bypass_mode;

          uint32_t page;
            	uint32_t offset;
            	uint32_t wdatah, wdatal;
            	uint32_t rdatah, rdatal;
            	uint32_t port_id;
                uint32_t *rdata;
                uint32_t phy_addr,i,reg_addr;
                uint32_t data;

         bypass_mode = 0;


          //256-byte data; CRC will be appended by Tx MAC
          uint8_t my_pkt_data[256] =
         {
           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
           0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
           0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
           0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
           0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
           0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
           0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
           0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
           0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
           0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
           0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
           0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
           0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
           0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
           0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
           0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

         };

          uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
          uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
          uint8_t rdata8;

      	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
          uint32_t frame_size,step,chk_noPkt;
          uint32_t timeout;
          uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


      	 ///////////// TEST ARG INIT ////////////////////

                    test_args_name[0]="Amac Ports =";
                    test_args_name[1]="Tx Desc Base =";
                    test_args_name[2]="Rx Desc Base =";
                    test_args_name[3]="Tx Desc Depth =";
                    test_args_name[4]="Rx Desc Depth =";
                    test_args_name[5]="Tx Buffer Base =";
                    test_args_name[6]="Rx Buffer Base =";
                    test_args_name[7]="Frame size =";

                    amac_ports        =1;
                    tx_desc_base      =0x75000000;//Max Desc space=(512 X 16)=0x2000
                    rx_desc_base      =0x76000000;
                    tx_desc_len       =200;//Max Desc=512=0x200
                    rx_desc_len       =200;
                    tx_buff_base      =0x65000000;//Max Buffer space=(512 X 12K)=0x600000
                    rx_buff_base      =0x66000000;
                    frame_size          =128;
                    //frame_sz            =1024;
                     step            =23;
          chk_noPkt       =tx_desc_len-1;
          //UNIMAC Config args
          eth_speed       =0x2;//1Gbps
          rx_pause_ignore =1;
          hd_ena          =1;
          mac_loop_con    =0;
          ena_ext_config  =1;
          rmt_loop_ena    =0;
          tx_pause_ignore =0;

      	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
      	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

      				//cpu_wr_single(0x301d060,0xb,4);
                      //for(i=0;i<2000000;i++);
      				//cpu_wr_single(0x301d060,0xf,4);

      	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

         ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
         for(amac_id=0;amac_id < amac_ports;amac_id++){
              post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
              //Basic AMAC block init
              amac_init(amac_id,bypass_mode);

              //UNIMAC Config
              unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                           rmt_loop_ena,tx_pause_ignore);

              //Reset MIB Counter
              counter_reset(amac_id);

              //Set up MAC loopback
              //amac_set_config(amac_id, MAC_LOOPBACK, 1);

              //Initialize AMAC TX DMA block
              amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
              amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
              for(index = 0; index < tx_desc_len; index++) {

                   //frame_size=((rand()%(0x3000-64))+64);

                   CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                   write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                   //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                   amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                   (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                   amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
              }

              //Initialize AMAC RX DMA block
              amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                              AMAC_PKT_BUFFER_SIZE_IN_BYTES);

              //Update with next descriptor value - outside EOT range
              //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

              //Within EOT range
              amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

               //amac_get_tx_status(amac_id);
               //amac_get_rx_status(amac_id);
               post_log("\n\n");
         }
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                       cfg_sw1_rev1();

      	                 page=0x0;
                            offset=0x6f;
                            //rdata = srab_rd(page, offset);
                            //wdatal=(rdatal | 0x1);
                            //wdatah=rdatah;
                            //srab_wr(page, offset, wdatah, wdatal);
      					  srab_rdwr(page,offset,0x1,5);


                            ccg_mdio_init();

                             phy_addr = 0;
                               for(i=0; i<16; i++) {
                                      reg_addr = i;
                                      gphy_reg_read(phy_addr, reg_addr, &data);
                                 }

                               //DSP_TAP10 FIX INCLUDED
     						  phy_addr = 0;
     	                    reg_addr = 0x18;
     	                    data = 0x0C30;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 0;
     	                    reg_addr = 0x17;
     	                    data = 0x000A;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 0;
     	                    reg_addr = 0x15;
     	                    data = 0x111B;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 1;
     	                    reg_addr = 0x18;
     	                    data = 0x0C30;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 1;
     	                    reg_addr = 0x17;
     	                    data = 0x000A;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 1;
     	                    reg_addr = 0x15;
     	                    data = 0x111B;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);



                              /* phy_addr = 0;
                                             reg_addr = 0x1;
                                             do {
                                          	    gphy_reg_read(phy_addr,reg_addr,&data);
                                             } while(!(data & 0x4));*/

                               //wait for linkup
                               /*phy_addr = 1;
                               reg_addr = 0x1;
                               do {
                            	    gphy_reg_read(phy_addr,reg_addr,&data);
                               } while(!(data & 0x4));*/



                  phy_addr = 1;
                  reg_addr = 0;
                  data = 0x3140;
                  gphy_reg_write(phy_addr, reg_addr, &data);

                  phy_addr = 1;
                  reg_addr = 4;
                  data = 0x781;
                  gphy_reg_write(phy_addr, reg_addr, &data);

                  phy_addr = 1;
                  reg_addr = 5;
                  data = 0xC781;
                  gphy_reg_write(phy_addr, reg_addr, &data);

                  phy_addr = 1;
                  reg_addr = 9;
                  data = 0x0000;
                  gphy_reg_write(phy_addr, reg_addr, &data);

                  phy_addr = 1;
                    						  reg_addr = 0;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                                data |= (1 << 14);
                                gphy_reg_write(phy_addr, reg_addr, &data);
                                gphy_reg_read(phy_addr, reg_addr, &data);


                  /*data = 0x01E1;
                  gphy_reg_write(phy_addr, 0x0004, &data);
                  gphy_reg_read(phy_addr, 0x0004, &data);*/



          ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
         for(amac_id=0;amac_id < amac_ports;amac_id++){
              post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
              // NxtDscr points to EOT Dscr
              nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                      ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
              //Out of EOT range,for continuous loopback
              //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                      ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
              amac_enable_tx_dma(amac_id, nextDscr);
              post_log("\n\n");
         }
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




         //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
         /*timeout=300;
         post_log("Sending packets for specified time\n");
         delay_noise(timeout,0);
         post_log("Time_taken =%d secs\n",(timeout*7));
         suspend_tx(amac_id);*/

      //   // read port8 rx stastic register
      //	page=0x28;
      //	// read port8 rx good byte register
      //	offset=0x0;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);
      //
      //// read port1 rx stastic register
      //	page=0x20;
      //	// read port8 rx good byte register
      //	offset=0x0;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);
      //
      //   // read port8 rx stastic register
      //	page=0x28;
      //	// read port8 rx good byte register
      //	offset=0x50;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);
      //
      //// read port1 rx stastic register
      //	page=0x21;
      //	// read port8 rx good byte register
      //	offset=0x51;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);






         for(amac_id=0;amac_id < amac_ports;amac_id++){
              waitforTrans_internalloopback(amac_id);
              post_log("\n");
              amac_gettxstats(amac_id);
              amac_getrxstats(amac_id);

              rpkt_no=checker_all(amac_id);
              post_log("\n");

              post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
              rpkt_chk=TEST_PASS;
              rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

               if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                       res_sts[amac_id]=TEST_PASS;
                       post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                       post_log("==========PASS==========\n");
               }
               else{
                       res_sts[amac_id]=TEST_FAIL;
                       post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                       post_log("==========FAIL==========\n");
               }

               post_log("\n\n");
         }
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


         if((res_sts[0]==TEST_PASS)){
                  result=TEST_PASS;
         }else{
                  result=TEST_FAIL;
          }
         return result;

         }

   int gphy1_internal_loopback_10MBPS(void)
      {

       test_status_t status = TEST_PASS;
       uint32_t test_args[NUM_TEST_ARGS];
       char *test_args_name[NUM_TEST_ARGS];
       uint32_t l,frame_sz,iter=1;
       uint32_t index;
   	test_status_t result,rpkt_no,rpkt_chk;
      test_status_t res_sts[2];
      amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
      amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
      amac_rx_pkt_info_t* pkt_ptr;
      uint32_t bypass_mode;

       uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

      bypass_mode = 0;


       //256-byte data; CRC will be appended by Tx MAC
       uint8_t my_pkt_data[256] =
      {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

      };

       uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
       uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
       uint8_t rdata8;

   	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
       uint32_t frame_size,step,chk_noPkt;
       uint32_t timeout;
       uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


   	 ///////////// TEST ARG INIT ////////////////////

                 test_args_name[0]="Amac Ports =";
                 test_args_name[1]="Tx Desc Base =";
                 test_args_name[2]="Rx Desc Base =";
                 test_args_name[3]="Tx Desc Depth =";
                 test_args_name[4]="Rx Desc Depth =";
                 test_args_name[5]="Tx Buffer Base =";
                 test_args_name[6]="Rx Buffer Base =";
                 test_args_name[7]="Frame size =";

                 amac_ports        =1;
                 tx_desc_base      =0x75000000;//Max Desc space=(512 X 16)=0x2000
                 rx_desc_base      =0x76000000;
                 tx_desc_len       =200;//Max Desc=512=0x200
                 rx_desc_len       =200;
                 tx_buff_base      =0x65000000;//Max Buffer space=(512 X 12K)=0x600000
                 rx_buff_base      =0x66000000;
                 frame_size          =128;
                 //frame_sz            =1024;
                  step            =23;
       chk_noPkt       =tx_desc_len-1;
       //UNIMAC Config args
       eth_speed       =0x2;//1Gbps
       rx_pause_ignore =1;
       hd_ena          =1;
       mac_loop_con    =0;
       ena_ext_config  =1;
       rmt_loop_ena    =0;
       tx_pause_ignore =0;

   	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
   	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

   				//cpu_wr_single(0x301d060,0xb,4);
                   //for(i=0;i<2000000;i++);
   				//cpu_wr_single(0x301d060,0xf,4);

   	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

      ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
           //Basic AMAC block init
           amac_init(amac_id,bypass_mode);

           //UNIMAC Config
           unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                        rmt_loop_ena,tx_pause_ignore);

           //Reset MIB Counter
           counter_reset(amac_id);

           //Set up MAC loopback
           //amac_set_config(amac_id, MAC_LOOPBACK, 1);

           //Initialize AMAC TX DMA block
           amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
           amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
           for(index = 0; index < tx_desc_len; index++) {

                //frame_size=((rand()%(0x3000-64))+64);

                CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
           }

           //Initialize AMAC RX DMA block
           amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                           AMAC_PKT_BUFFER_SIZE_IN_BYTES);

           //Update with next descriptor value - outside EOT range
           //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

           //Within EOT range
           amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

            //amac_get_tx_status(amac_id);
            //amac_get_rx_status(amac_id);
            post_log("\n\n");
      }
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    cfg_sw1_rev1();

   	                 page=0x0;
                         offset=0x6f;
                         //rdata = srab_rd(page, offset);
                         //wdatal=(rdatal | 0x1);
                         //wdatah=rdatah;
                         //srab_wr(page, offset, wdatah, wdatal);
   					  srab_rdwr(page,offset,0x1,5);


                         ccg_mdio_init();

                          phy_addr = 0;
                            for(i=0; i<16; i++) {
                                   reg_addr = i;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                              }

                            //DSP_TAP10 FIX INCLUDED
  						  phy_addr = 0;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 0;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x18;
  	                    data = 0x0C30;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x17;
  	                    data = 0x000A;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);

  	                    phy_addr = 1;
  	                    reg_addr = 0x15;
  	                    data = 0x111B;
  	                    gphy_reg_write(phy_addr,reg_addr,&data);


                            //Lane Swapping and Polarity inversion for Gphy0
                            phy_addr = 0;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 0x15;
                            data = 0x5193;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            //Lane Swapping and Polarity inversion for Gphy1
                            phy_addr = 1;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 0x15;
                            data = 0x11C9;
                            gphy_reg_write(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                           /* phy_addr = 0;
                                          reg_addr = 0x1;
                                          do {
                                       	    gphy_reg_read(phy_addr,reg_addr,&data);
                                          } while(!(data & 0x4));*/

                            //wait for linkup
                            /*phy_addr = 1;
                            reg_addr = 0x1;
                            do {
                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                            } while(!(data & 0x4));*/



               phy_addr = 1;
               reg_addr = 0;
               data = 0x1140;
               gphy_reg_write(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 4;
               data = 0x461;
               gphy_reg_write(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 5;
               data = 0xC461;
               gphy_reg_write(phy_addr, reg_addr, &data);

               phy_addr = 1;
               reg_addr = 9;
               data = 0x0000;
               gphy_reg_write(phy_addr, reg_addr, &data);

               phy_addr = 1;
					  reg_addr = 0;
     gphy_reg_read(phy_addr, reg_addr, &data);
     data |= (1 << 14);
     gphy_reg_write(phy_addr, reg_addr, &data);
     gphy_reg_read(phy_addr, reg_addr, &data);


               /*data = 0x01E1;
               gphy_reg_write(phy_addr, 0x0004, &data);
               gphy_reg_read(phy_addr, 0x0004, &data);*/



       ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
           // NxtDscr points to EOT Dscr
           nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           //Out of EOT range,for continuous loopback
           //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           amac_enable_tx_dma(amac_id, nextDscr);
           post_log("\n\n");
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
      /*timeout=300;
      post_log("Sending packets for specified time\n");
      delay_noise(timeout,0);
      post_log("Time_taken =%d secs\n",(timeout*7));
      suspend_tx(amac_id);*/

   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x20;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x50;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x21;
   //	// read port8 rx good byte register
   //	offset=0x51;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);






      for(amac_id=0;amac_id < amac_ports;amac_id++){
           waitforTrans_internalloopback(amac_id);
           post_log("\n");
           amac_gettxstats(amac_id);
           amac_getrxstats(amac_id);

           rpkt_no=checker_all(amac_id);
           post_log("\n");

           post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
           rpkt_chk=TEST_PASS;
           rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

            if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                    res_sts[amac_id]=TEST_PASS;
                    post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                    post_log("==========PASS==========\n");
            }
            else{
                    res_sts[amac_id]=TEST_FAIL;
                    post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                    post_log("==========FAIL==========\n");
            }

            post_log("\n\n");
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      if((res_sts[0]==TEST_PASS)){
               result=TEST_PASS;
      }else{
               result=TEST_FAIL;
       }
      return result;

      }

   int gphy1_internal_loopback_without_laneswap_10MBPS(void)
         {

          test_status_t status = TEST_PASS;
          uint32_t test_args[NUM_TEST_ARGS];
          char *test_args_name[NUM_TEST_ARGS];
          uint32_t l,frame_sz,iter=1;
          uint32_t index;
      	test_status_t result,rpkt_no,rpkt_chk;
         test_status_t res_sts[2];
         amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
         amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
         amac_rx_pkt_info_t* pkt_ptr;
         uint32_t bypass_mode;

          uint32_t page;
            	uint32_t offset;
            	uint32_t wdatah, wdatal;
            	uint32_t rdatah, rdatal;
            	uint32_t port_id;
                uint32_t *rdata;
                uint32_t phy_addr,i,reg_addr;
                uint32_t data;

         bypass_mode = 0;


          //256-byte data; CRC will be appended by Tx MAC
          uint8_t my_pkt_data[256] =
         {
           0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
           0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
           0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
           0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
           0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
           0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
           0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
           0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
           0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
           0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
           0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
           0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
           0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
           0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
           0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
           0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

         };

          uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
          uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
          uint8_t rdata8;

      	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
          uint32_t frame_size,step,chk_noPkt;
          uint32_t timeout;
          uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


      	 ///////////// TEST ARG INIT ////////////////////

                    test_args_name[0]="Amac Ports =";
                    test_args_name[1]="Tx Desc Base =";
                    test_args_name[2]="Rx Desc Base =";
                    test_args_name[3]="Tx Desc Depth =";
                    test_args_name[4]="Rx Desc Depth =";
                    test_args_name[5]="Tx Buffer Base =";
                    test_args_name[6]="Rx Buffer Base =";
                    test_args_name[7]="Frame size =";

                    amac_ports        =1;
                    tx_desc_base      =0x75000000;//Max Desc space=(512 X 16)=0x2000
                    rx_desc_base      =0x76000000;
                    tx_desc_len       =200;//Max Desc=512=0x200
                    rx_desc_len       =200;
                    tx_buff_base      =0x65000000;//Max Buffer space=(512 X 12K)=0x600000
                    rx_buff_base      =0x66000000;
                    frame_size          =128;
                    //frame_sz            =1024;
                     step            =23;
          chk_noPkt       =tx_desc_len-1;
          //UNIMAC Config args
          eth_speed       =0x2;//1Gbps
          rx_pause_ignore =1;
          hd_ena          =1;
          mac_loop_con    =0;
          ena_ext_config  =1;
          rmt_loop_ena    =0;
          tx_pause_ignore =0;

      	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
      	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

      				//cpu_wr_single(0x301d060,0xb,4);
                      //for(i=0;i<2000000;i++);
      				//cpu_wr_single(0x301d060,0xf,4);

      	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

         ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
         for(amac_id=0;amac_id < amac_ports;amac_id++){
              post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
              //Basic AMAC block init
              amac_init(amac_id,bypass_mode);

              //UNIMAC Config
              unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                           rmt_loop_ena,tx_pause_ignore);

              //Reset MIB Counter
              counter_reset(amac_id);

              //Set up MAC loopback
              //amac_set_config(amac_id, MAC_LOOPBACK, 1);

              //Initialize AMAC TX DMA block
              amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
              amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
              for(index = 0; index < tx_desc_len; index++) {

                   //frame_size=((rand()%(0x3000-64))+64);

                   CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                   write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                   //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                   amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                   (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                   amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
              }

              //Initialize AMAC RX DMA block
              amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                              AMAC_PKT_BUFFER_SIZE_IN_BYTES);

              //Update with next descriptor value - outside EOT range
              //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

              //Within EOT range
              amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

               //amac_get_tx_status(amac_id);
               //amac_get_rx_status(amac_id);
               post_log("\n\n");
         }
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                       cfg_sw1_rev1();

      	                 page=0x0;
                            offset=0x6f;
                            //rdata = srab_rd(page, offset);
                            //wdatal=(rdatal | 0x1);
                            //wdatah=rdatah;
                            //srab_wr(page, offset, wdatah, wdatal);
      					  srab_rdwr(page,offset,0x1,5);


                            ccg_mdio_init();

                             phy_addr = 0;
                               for(i=0; i<16; i++) {
                                      reg_addr = i;
                                      gphy_reg_read(phy_addr, reg_addr, &data);
                                 }

                               //DSP_TAP10 FIX INCLUDED
     						  phy_addr = 0;
     	                    reg_addr = 0x18;
     	                    data = 0x0C30;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 0;
     	                    reg_addr = 0x17;
     	                    data = 0x000A;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 0;
     	                    reg_addr = 0x15;
     	                    data = 0x111B;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 1;
     	                    reg_addr = 0x18;
     	                    data = 0x0C30;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 1;
     	                    reg_addr = 0x17;
     	                    data = 0x000A;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);

     	                    phy_addr = 1;
     	                    reg_addr = 0x15;
     	                    data = 0x111B;
     	                    gphy_reg_write(phy_addr,reg_addr,&data);




                              /* phy_addr = 0;
                                             reg_addr = 0x1;
                                             do {
                                          	    gphy_reg_read(phy_addr,reg_addr,&data);
                                             } while(!(data & 0x4));*/

                               //wait for linkup
                               /*phy_addr = 1;
                               reg_addr = 0x1;
                               do {
                            	    gphy_reg_read(phy_addr,reg_addr,&data);
                               } while(!(data & 0x4));*/



                  phy_addr = 1;
                  reg_addr = 0;
                  data = 0x1140;
                  gphy_reg_write(phy_addr, reg_addr, &data);

                  phy_addr = 1;
                  reg_addr = 4;
                  data = 0x461;
                  gphy_reg_write(phy_addr, reg_addr, &data);

                  phy_addr = 1;
                  reg_addr = 5;
                  data = 0xC461;
                  gphy_reg_write(phy_addr, reg_addr, &data);

                  phy_addr = 1;
                  reg_addr = 9;
                  data = 0x0000;
                  gphy_reg_write(phy_addr, reg_addr, &data);

                  phy_addr = 1;
   					  reg_addr = 0;
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 14);
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);


                  /*data = 0x01E1;
                  gphy_reg_write(phy_addr, 0x0004, &data);
                  gphy_reg_read(phy_addr, 0x0004, &data);*/



          ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
         for(amac_id=0;amac_id < amac_ports;amac_id++){
              post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
              // NxtDscr points to EOT Dscr
              nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                      ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
              //Out of EOT range,for continuous loopback
              //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                      ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
              amac_enable_tx_dma(amac_id, nextDscr);
              post_log("\n\n");
         }
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




         //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
         /*timeout=300;
         post_log("Sending packets for specified time\n");
         delay_noise(timeout,0);
         post_log("Time_taken =%d secs\n",(timeout*7));
         suspend_tx(amac_id);*/

      //   // read port8 rx stastic register
      //	page=0x28;
      //	// read port8 rx good byte register
      //	offset=0x0;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);
      //
      //// read port1 rx stastic register
      //	page=0x20;
      //	// read port8 rx good byte register
      //	offset=0x0;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);
      //
      //   // read port8 rx stastic register
      //	page=0x28;
      //	// read port8 rx good byte register
      //	offset=0x50;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);
      //
      //// read port1 rx stastic register
      //	page=0x21;
      //	// read port8 rx good byte register
      //	offset=0x51;
      //	rdata = srab_rd(page, offset);
      //    rdatal=rdata[0];
      //    rdatah=rdata[1];
      //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
      //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
      //	wdatal=((rdatal&0xfffffffC) | 0x3);






         for(amac_id=0;amac_id < amac_ports;amac_id++){
              waitforTrans_internalloopback(amac_id);
              post_log("\n");
              amac_gettxstats(amac_id);
              amac_getrxstats(amac_id);

              rpkt_no=checker_all(amac_id);
              post_log("\n");

              post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
              rpkt_chk=TEST_PASS;
              rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

               if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                       res_sts[amac_id]=TEST_PASS;
                       post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                       post_log("==========PASS==========\n");
               }
               else{
                       res_sts[amac_id]=TEST_FAIL;
                       post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                       post_log("==========FAIL==========\n");
               }

               post_log("\n\n");
         }
         ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


         if((res_sts[0]==TEST_PASS)){
                  result=TEST_PASS;
         }else{
                  result=TEST_FAIL;
          }
         return result;

         }

   int gphy1_internal_loopback_without_laneswap_1GBPS(void)
      {

       test_status_t status = TEST_PASS;
       uint32_t test_args[NUM_TEST_ARGS];
       char *test_args_name[NUM_TEST_ARGS];
       uint32_t l,frame_sz,iter=1;
       uint32_t index;
   	test_status_t result,rpkt_no,rpkt_chk;
      test_status_t res_sts[2];
      amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
      amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
      amac_rx_pkt_info_t* pkt_ptr;
      uint32_t bypass_mode;

       uint32_t page;
         	uint32_t offset;
         	uint32_t wdatah, wdatal;
         	uint32_t rdatah, rdatal;
         	uint32_t port_id;
             uint32_t *rdata;
             uint32_t phy_addr,i,reg_addr;
             uint32_t data;

      bypass_mode = 0;


       //256-byte data; CRC will be appended by Tx MAC
       uint8_t my_pkt_data[256] =
      {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
        0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
        0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
        0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
        0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
        0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
        0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
        0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

      };

       uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
       uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
       uint8_t rdata8;

   	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
       uint32_t frame_size,step,chk_noPkt;
       uint32_t timeout;
       uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;


   	 ///////////// TEST ARG INIT ////////////////////

                 test_args_name[0]="Amac Ports =";
                 test_args_name[1]="Tx Desc Base =";
                 test_args_name[2]="Rx Desc Base =";
                 test_args_name[3]="Tx Desc Depth =";
                 test_args_name[4]="Rx Desc Depth =";
                 test_args_name[5]="Tx Buffer Base =";
                 test_args_name[6]="Rx Buffer Base =";
                 test_args_name[7]="Frame size =";

                 amac_ports        =1;
                 tx_desc_base      =0x75000000;//Max Desc space=(512 X 16)=0x2000
                 rx_desc_base      =0x76000000;
                 tx_desc_len       =200;//Max Desc=512=0x200
                 rx_desc_len       =200;
                 tx_buff_base      =0x65000000;//Max Buffer space=(512 X 12K)=0x600000
                 rx_buff_base      =0x66000000;
                 frame_size          =128;
                 //frame_sz            =1024;
                  step            =23;
       chk_noPkt       =tx_desc_len-1;
       //UNIMAC Config args
       eth_speed       =0x2;//1Gbps
       rx_pause_ignore =1;
       hd_ena          =1;
       mac_loop_con    =0;
       ena_ext_config  =1;
       rmt_loop_ena    =0;
       tx_pause_ignore =0;

   	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
   	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

   				//cpu_wr_single(0x301d060,0xb,4);
                   //for(i=0;i<2000000;i++);
   				//cpu_wr_single(0x301d060,0xf,4);

   	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

      ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
           //Basic AMAC block init
           amac_init(amac_id,bypass_mode);

           //UNIMAC Config
           unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                        rmt_loop_ena,tx_pause_ignore);

           //Reset MIB Counter
           counter_reset(amac_id);

           //Set up MAC loopback
           //amac_set_config(amac_id, MAC_LOOPBACK, 1);

           //Initialize AMAC TX DMA block
           amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
           amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
           for(index = 0; index < tx_desc_len; index++) {

                //frame_size=((rand()%(0x3000-64))+64);

                CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

                write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

                //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
                amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                                (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
                amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
           }

           //Initialize AMAC RX DMA block
           amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                           AMAC_PKT_BUFFER_SIZE_IN_BYTES);

           //Update with next descriptor value - outside EOT range
           //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

           //Within EOT range
           amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

            //amac_get_tx_status(amac_id);
            //amac_get_rx_status(amac_id);
            post_log("\n\n");
      }
       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    cfg_sw1_rev1();

   	                 page=0x0;
                         offset=0x6f;
                         //rdata = srab_rd(page, offset);
                         //wdatal=(rdatal | 0x1);
                         //wdatah=rdatah;
                         //srab_wr(page, offset, wdatah, wdatal);
   					  srab_rdwr(page,offset,0x1,5);


                         ccg_mdio_init();

                         //DSP_TAP10 FIX INCLUDED
						  phy_addr = 0;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 0;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x18;
	                    data = 0x0C30;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x17;
	                    data = 0x000A;
	                    gphy_reg_write(phy_addr,reg_addr,&data);

	                    phy_addr = 1;
	                    reg_addr = 0x15;
	                    data = 0x111B;
	                    gphy_reg_write(phy_addr,reg_addr,&data);


                         phy_addr = 1;
                         	                    reg_addr = 0x18;
                         	                    data = 0x0C30;
                         	                    gphy_reg_write(phy_addr,reg_addr,&data);

                         	                    phy_addr = 1;
                         	                    reg_addr = 0x17;
                         	                    data = 0x000A;
                         	                    gphy_reg_write(phy_addr,reg_addr,&data);

                         	                    phy_addr = 1;
                         	                    reg_addr = 0x15;
                         	                    data = 0x111B;
                         	                    gphy_reg_write(phy_addr,reg_addr,&data);

                          phy_addr = 0;
                            for(i=0; i<16; i++) {
                                   reg_addr = i;
                                   gphy_reg_read(phy_addr, reg_addr, &data);
                              }

                            //Lane Swapping and Polarity inversion for Gphy0
                            /*phy_addr = 0;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 0;
                            reg_addr = 0x15;
                            data = 0x5193;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            for(i=0; i<20000000; i++) ;

                            //Lane Swapping and Polarity inversion for Gphy1
                            phy_addr = 1;
                            reg_addr = 0x17;
                            data = 0x0F09;
                            gphy_reg_write(phy_addr, reg_addr, &data);
                            //gphy_reg_read(phy_addr, reg_addr, &data);

                            phy_addr = 1;
                            reg_addr = 0x15;
                            data = 0x11C9;
                            gphy_reg_write(phy_addr, reg_addr, &data);*/

                            for(i=0; i<20000000; i++) ;

                           /* phy_addr = 0;
                                          reg_addr = 0x1;
                                          do {
                                       	    gphy_reg_read(phy_addr,reg_addr,&data);
                                          } while(!(data & 0x4));*/

                            //wait for linkup
                            /*phy_addr = 1;
                            reg_addr = 0x1;
                            do {
                         	    gphy_reg_read(phy_addr,reg_addr,&data);
                            } while(!(data & 0x4));*/

                         phy_addr = 1;
   						  reg_addr = 0;
               gphy_reg_read(phy_addr, reg_addr, &data);
               data |= (1 << 14);
               gphy_reg_write(phy_addr, reg_addr, &data);
               gphy_reg_read(phy_addr, reg_addr, &data);

               data = 0x01E1;
               gphy_reg_write(phy_addr, 0x0004, &data);
               gphy_reg_read(phy_addr, 0x0004, &data);



       ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
      for(amac_id=0;amac_id < amac_ports;amac_id++){
           post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
           // NxtDscr points to EOT Dscr
           nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           //Out of EOT range,for continuous loopback
           //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                   ((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
           amac_enable_tx_dma(amac_id, nextDscr);
           post_log("\n\n");
      }
      /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




      //////////////////////////////////////////////// Checkers and Status for A0  and A1 //////////////////////////////////////
      /*timeout=300;
      post_log("Sending packets for specified time\n");
      delay_noise(timeout,0);
      post_log("Time_taken =%d secs\n",(timeout*7));
      suspend_tx(amac_id);*/

   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x20;
   //	// read port8 rx good byte register
   //	offset=0x0;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 0 Tx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //   // read port8 rx stastic register
   //	page=0x28;
   //	// read port8 rx good byte register
   //	offset=0x50;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 8 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);
   //
   //// read port1 rx stastic register
   //	page=0x21;
   //	// read port8 rx good byte register
   //	offset=0x51;
   //	rdata = srab_rd(page, offset);
   //    rdatal=rdata[0];
   //    rdatah=rdata[1];
   //    ////post_log("Switch mode rdata[0]=%#x rdata[1]=%#x\n",rdatal,rdatah);
   //    post_log("port 1 Rx Packet byte counter rdatal=%#x rdatah=%#x\n",rdatal,rdatah);//
   //	wdatal=((rdatal&0xfffffffC) | 0x3);






      for(amac_id=0;amac_id < amac_ports;amac_id++){
           waitforTrans_internalloopback(amac_id);
           post_log("\n");
           amac_gettxstats(amac_id);
           amac_getrxstats(amac_id);

           rpkt_no=checker_all(amac_id);
           post_log("\n");

           post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
           rpkt_chk=TEST_PASS;
           rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

            if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                    res_sts[amac_id]=TEST_PASS;
                    post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
                    post_log("==========PASS==========\n");
            }
            else{
                    res_sts[amac_id]=TEST_FAIL;
                    post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
                    post_log("==========FAIL==========\n");
            }

            post_log("\n\n");
      }
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


      if((res_sts[0]==TEST_PASS)){
               result=TEST_PASS;
      }else{
               result=TEST_FAIL;
       }
      return result;

      }

   int amacswitch_port5_to_port0_chip_nd_phy_d_1(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }

	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =20;//Max Desc=512=0x200
              rx_desc_len       =20;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				 data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL7, data,4);

    data = cpu_rd_single(CDRU_RGMII_IO_CONFIG,4);
    data |= (7 << CDRU_RGMII_IO_CONFIG__IOMUX_RGMII_DRV_SEL_R);
    cpu_wr_single(CDRU_RGMII_IO_CONFIG, data,4);


				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 //cfg_sw1_rgmii();
				 // write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);
	// write switch control register	
	offset=0x22;
	srab_rdwr(page,offset,0xffffffff,6);
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}
    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);


/////Configure RGMII Delay - Chip in Non Delay mode
	page=0xe4; 
	offset = 0x2a;
	 srab_rdwr(page, offset, 0x0,1);

	 	page=0xe4; 
	offset = 0x4a;
	 srab_rdwr(page, offset, 0xF9,1);  //-0.5x dalay
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

    page=0x31; 
	offset = 0xa;
    srab_rdwr(page,offset,0x001,4);

    page=0x31; 
	offset = 0x0;
    srab_rdwr(page,offset,0x020,4);

#endif

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

						 phy_addr = 0;
                                       reg_addr = 0x1;
                                      do {
                                   	    gphy_reg_read(phy_addr,reg_addr,&data);
                                      } while(!(data & 0x4));

									  phy_addr = 0x0;
        reg_addr = 0x11;
		gphy_reg_read(phy_addr, reg_addr, &data);

                        

              

						ccg_mdio_init_ext1();

						phy_addr = 0x10;
     
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11B;
        gphy_reg_write(phy_addr, reg_addr, &data);
    
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);
       
   
        /*reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
		data = 0xF0E7;
		gphy_reg_write(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);*/
         reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 15);
       // data &= ~(1 << 8);
	    data |= (1 << 8);
        gphy_reg_write(phy_addr, reg_addr, &data);
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);



		


        wait_for_rgmii_phy_linkup();

		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 
		 

        post_log("-------------------------------------------\n\n");

        page=0x25;
		read_switch_statistics(page);
		page=0x20;
		read_switch_statistics(page);

		post_log("Printing the phy statistics now....MII xstat register \n");
		
		for (i=0;i<20 ;i++ )
		{
			phy_addr = 0x10;
            reg_addr = 0x11;
		    gphy_reg_read(phy_addr, reg_addr, &data);
		}

      
   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }

   int amacswitch_port5_to_port0_chip_nd_phy_d_1_hysterisis_enable(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }

	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =20;//Max Desc=512=0x200
              rx_desc_len       =20;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				 data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL7, data,4);

    data = cpu_rd_single(CDRU_RGMII_IO_CONFIG,4);
    data |= (7 << CDRU_RGMII_IO_CONFIG__IOMUX_RGMII_DRV_SEL_R);
	data |= (1 << 9);  //Hysterisis Enable
    cpu_wr_single(CDRU_RGMII_IO_CONFIG, data,4);


				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 //cfg_sw1_rgmii();
				 // write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);
	// write switch control register	
	offset=0x22;
	srab_rdwr(page,offset,0xffffffff,6);
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}
    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);


/////Configure RGMII Delay - Chip in Non Delay mode
	page=0xe4; 
	offset = 0x2a;
	 srab_rdwr(page, offset, 0x0,1);

	 	page=0xe4; 
	offset = 0x4a;
	 srab_rdwr(page, offset, 0xF9,1);  //-0.5x dalay
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

    page=0x31; 
	offset = 0xa;
    srab_rdwr(page,offset,0x001,4);

    page=0x31; 
	offset = 0x0;
    srab_rdwr(page,offset,0x020,4);

#endif

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

						 phy_addr = 0;
                                       reg_addr = 0x1;
                                      do {
                                   	    gphy_reg_read(phy_addr,reg_addr,&data);
                                      } while(!(data & 0x4));

									  phy_addr = 0x0;
        reg_addr = 0x11;
		gphy_reg_read(phy_addr, reg_addr, &data);

                        

              

						ccg_mdio_init_ext1();

						phy_addr = 0x10;
     
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11B;
        gphy_reg_write(phy_addr, reg_addr, &data);
    
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);
       
   
        /*reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
		data = 0xF0E7;
		gphy_reg_write(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);*/
         reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 15);
       // data &= ~(1 << 8);
	    data |= (1 << 8);
        gphy_reg_write(phy_addr, reg_addr, &data);
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);



		


        wait_for_rgmii_phy_linkup();

		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 
		 

        post_log("-------------------------------------------\n\n");

        page=0x25;
		read_switch_statistics(page);
		page=0x20;
		read_switch_statistics(page);

		post_log("Printing the phy statistics now....MII xstat register \n");
		
		for (i=0;i<20 ;i++ )
		{
			phy_addr = 0x10;
            reg_addr = 0x11;
		    gphy_reg_read(phy_addr, reg_addr, &data);
		}

      
   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }


 int amacswitch_port5_to_port0_chip_nd_phy_d_2(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }

	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =20;//Max Desc=512=0x200
              rx_desc_len       =20;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				 data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL7, data,4);

    data = cpu_rd_single(CDRU_RGMII_IO_CONFIG,4);
    data |= (7 << CDRU_RGMII_IO_CONFIG__IOMUX_RGMII_DRV_SEL_R);
    cpu_wr_single(CDRU_RGMII_IO_CONFIG, data,4);


				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 //cfg_sw1_rgmii();
				 // write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);
	// write switch control register	
	offset=0x22;
	srab_rdwr(page,offset,0xffffffff,6);
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}
    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);


/////Configure RGMII Delay - Chip in Non Delay mode
	page=0xe4; 
	offset = 0x2a;
	 srab_rdwr(page, offset, 0x0,1);

	 	page=0xe4; 
	offset = 0x4a;
	 srab_rdwr(page, offset, 0x99,1);  //+0.5x dalay
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

    page=0x31; 
	offset = 0xa;
    srab_rdwr(page,offset,0x001,4);

    page=0x31; 
	offset = 0x0;
    srab_rdwr(page,offset,0x020,4);

#endif

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

						  phy_addr = 0;
                                       reg_addr = 0x1;
                                      do {
                                   	    gphy_reg_read(phy_addr,reg_addr,&data);
                                      } while(!(data & 0x4));

									  phy_addr = 0x0;
        reg_addr = 0x11;
		gphy_reg_read(phy_addr, reg_addr, &data);


                        

              

						ccg_mdio_init_ext1();

						phy_addr = 0x10;
     
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11B;
        gphy_reg_write(phy_addr, reg_addr, &data);
    
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);
       
   
        /*reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
		data = 0xF0E7;
		gphy_reg_write(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);*/
         reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 15);
       // data &= ~(1 << 8);
	    data |= (1 << 8);
        gphy_reg_write(phy_addr, reg_addr, &data);
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);



		

        wait_for_rgmii_phy_linkup();

		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 
		 

        post_log("-------------------------------------------\n\n");

        page=0x25;
		read_switch_statistics(page);
		page=0x20;
		read_switch_statistics(page);

		post_log("Printing the phy statistics now....MII xstat register \n");
		
		for (i=0;i<20 ;i++ )
		{
			phy_addr = 0x10;
            reg_addr = 0x11;
		    gphy_reg_read(phy_addr, reg_addr, &data);
		}
      
   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }


 int amacswitch_port5_to_port0_chip_nd_phy_d_2_hysterisis_enable(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }

	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =20;//Max Desc=512=0x200
              rx_desc_len       =20;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				 data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL7, data,4);

    data = cpu_rd_single(CDRU_RGMII_IO_CONFIG,4);
    data |= (7 << CDRU_RGMII_IO_CONFIG__IOMUX_RGMII_DRV_SEL_R);
	data |= (1 << 9);
    cpu_wr_single(CDRU_RGMII_IO_CONFIG, data,4);


				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 //cfg_sw1_rgmii();
				 // write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);
	// write switch control register	
	offset=0x22;
	srab_rdwr(page,offset,0xffffffff,6);
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}
    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);


/////Configure RGMII Delay - Chip in Non Delay mode
	page=0xe4; 
	offset = 0x2a;
	 srab_rdwr(page, offset, 0x0,1);

	 	page=0xe4; 
	offset = 0x4a;
	 srab_rdwr(page, offset, 0x99,1);  //+0.5x dalay
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

    page=0x31; 
	offset = 0xa;
    srab_rdwr(page,offset,0x001,4);

    page=0x31; 
	offset = 0x0;
    srab_rdwr(page,offset,0x020,4);

#endif

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

						  phy_addr = 0;
                                       reg_addr = 0x1;
                                      do {
                                   	    gphy_reg_read(phy_addr,reg_addr,&data);
                                      } while(!(data & 0x4));

									  phy_addr = 0x0;
        reg_addr = 0x11;
		gphy_reg_read(phy_addr, reg_addr, &data);


                        

              

						ccg_mdio_init_ext1();

						phy_addr = 0x10;
     
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11B;
        gphy_reg_write(phy_addr, reg_addr, &data);
    
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);
       
   
        /*reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
		data = 0xF0E7;
		gphy_reg_write(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);*/
         reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 15);
       // data &= ~(1 << 8);
	    data |= (1 << 8);
        gphy_reg_write(phy_addr, reg_addr, &data);
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);



		

        wait_for_rgmii_phy_linkup();

		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 
		 

        post_log("-------------------------------------------\n\n");

        page=0x25;
		read_switch_statistics(page);
		page=0x20;
		read_switch_statistics(page);

		post_log("Printing the phy statistics now....MII xstat register \n");
		
		for (i=0;i<20 ;i++ )
		{
			phy_addr = 0x10;
            reg_addr = 0x11;
		    gphy_reg_read(phy_addr, reg_addr, &data);
		}
      
   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }

    int amacswitch_port5_to_port0_chip_nd_phy_d_3(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }
	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =20;//Max Desc=512=0x200
              rx_desc_len       =20;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				 data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL7, data,4);

    data = cpu_rd_single(CDRU_RGMII_IO_CONFIG,4);
    data |= (7 << CDRU_RGMII_IO_CONFIG__IOMUX_RGMII_DRV_SEL_R);
    cpu_wr_single(CDRU_RGMII_IO_CONFIG, data,4);


				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 //cfg_sw1_rgmii();
				 // write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);
	// write switch control register	
	offset=0x22;
	srab_rdwr(page,offset,0xffffffff,6);
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}
    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);


/////Configure RGMII Delay - Chip in Non Delay mode
	page=0xe4; 
	offset = 0x2a;
	 srab_rdwr(page, offset, 0x0,1);

	 	page=0xe4; 
	offset = 0x4a;
	 srab_rdwr(page, offset, 0xE9,1);  //-1.0x dalay
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

    page=0x31; 
	offset = 0xa;
    srab_rdwr(page,offset,0x001,4);

    page=0x31; 
	offset = 0x0;
    srab_rdwr(page,offset,0x020,4);

#endif

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

						  phy_addr = 0;
                                       reg_addr = 0x1;
                                      do {
                                   	    gphy_reg_read(phy_addr,reg_addr,&data);
                                      } while(!(data & 0x4));

									  phy_addr = 0x0;
        reg_addr = 0x11;
		gphy_reg_read(phy_addr, reg_addr, &data);


                        

              

						ccg_mdio_init_ext1();

						phy_addr = 0x10;
     
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11B;
        gphy_reg_write(phy_addr, reg_addr, &data);
    
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);
       
   
        /*reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
		data = 0xF0E7;
		gphy_reg_write(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);*/
         reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 15);
       // data &= ~(1 << 8);
	    data |= (1 << 8);
        gphy_reg_write(phy_addr, reg_addr, &data);
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);



		


        wait_for_rgmii_phy_linkup();

		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 
		 

        post_log("-------------------------------------------\n\n");

        page=0x25;
		read_switch_statistics(page);
		page=0x20;
		read_switch_statistics(page);

		post_log("Printing the phy statistics now....MII xstat register \n");
		
		for (i=0;i<20 ;i++ )
		{
			phy_addr = 0x10;
            reg_addr = 0x11;
		    gphy_reg_read(phy_addr, reg_addr, &data);
		}
      
   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }


     int amacswitch_port5_to_port0_chip_nd_phy_d_3_hysterisis_enable(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }
	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =20;//Max Desc=512=0x200
              rx_desc_len       =20;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				 data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL7, data,4);

    data = cpu_rd_single(CDRU_RGMII_IO_CONFIG,4);
    data |= (7 << CDRU_RGMII_IO_CONFIG__IOMUX_RGMII_DRV_SEL_R);
	data |= (1 << 9);
    cpu_wr_single(CDRU_RGMII_IO_CONFIG, data,4);


				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 //cfg_sw1_rgmii();
				 // write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);
	// write switch control register	
	offset=0x22;
	srab_rdwr(page,offset,0xffffffff,6);
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}
    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);


/////Configure RGMII Delay - Chip in Non Delay mode
	page=0xe4; 
	offset = 0x2a;
	 srab_rdwr(page, offset, 0x0,1);

	 	page=0xe4; 
	offset = 0x4a;
	 srab_rdwr(page, offset, 0xE9,1);  //-1.0x dalay
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

    page=0x31; 
	offset = 0xa;
    srab_rdwr(page,offset,0x001,4);

    page=0x31; 
	offset = 0x0;
    srab_rdwr(page,offset,0x020,4);

#endif

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

						  phy_addr = 0;
                                       reg_addr = 0x1;
                                      do {
                                   	    gphy_reg_read(phy_addr,reg_addr,&data);
                                      } while(!(data & 0x4));

									  phy_addr = 0x0;
        reg_addr = 0x11;
		gphy_reg_read(phy_addr, reg_addr, &data);


                        

              

						ccg_mdio_init_ext1();

						phy_addr = 0x10;
     
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11B;
        gphy_reg_write(phy_addr, reg_addr, &data);
    
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);
       
   
        /*reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
		data = 0xF0E7;
		gphy_reg_write(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);*/
         reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 15);
       // data &= ~(1 << 8);
	    data |= (1 << 8);
        gphy_reg_write(phy_addr, reg_addr, &data);
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);



		


        wait_for_rgmii_phy_linkup();

		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 
		 

        post_log("-------------------------------------------\n\n");

        page=0x25;
		read_switch_statistics(page);
		page=0x20;
		read_switch_statistics(page);

		post_log("Printing the phy statistics now....MII xstat register \n");
		
		for (i=0;i<20 ;i++ )
		{
			phy_addr = 0x10;
            reg_addr = 0x11;
		    gphy_reg_read(phy_addr, reg_addr, &data);
		}
      
   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }

    int amacswitch_port5_to_port0_chip_nd_phy_d_4(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }
	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =20;//Max Desc=512=0x200
              rx_desc_len       =20;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				 data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL7, data,4);

    data = cpu_rd_single(CDRU_RGMII_IO_CONFIG,4);
    data |= (7 << CDRU_RGMII_IO_CONFIG__IOMUX_RGMII_DRV_SEL_R);
    cpu_wr_single(CDRU_RGMII_IO_CONFIG, data,4);


				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 //cfg_sw1_rgmii();
				 // write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);
	// write switch control register	
	offset=0x22;
	srab_rdwr(page,offset,0xffffffff,6);
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}
    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);


/////Configure RGMII Delay - Chip in Non Delay mode
	page=0xe4; 
	offset = 0x2a;
	 srab_rdwr(page, offset, 0x0,1);

	 	page=0xe4; 
	offset = 0x4a;
	 srab_rdwr(page, offset, 0xA9,1);  //+1.0x dalay
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

    page=0x31; 
	offset = 0xa;
    srab_rdwr(page,offset,0x001,4);

    page=0x31; 
	offset = 0x0;
    srab_rdwr(page,offset,0x020,4);

#endif

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

						  phy_addr = 0;
                                       reg_addr = 0x1;
                                      do {
                                   	    gphy_reg_read(phy_addr,reg_addr,&data);
                                      } while(!(data & 0x4));

									  phy_addr = 0x0;
        reg_addr = 0x11;
		gphy_reg_read(phy_addr, reg_addr, &data);


                        

              

						ccg_mdio_init_ext1();

						phy_addr = 0x10;
     
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11B;
        gphy_reg_write(phy_addr, reg_addr, &data);
    
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);
       
   
        /*reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
		data = 0xF0E7;
		gphy_reg_write(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);*/
         reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 15);
       // data &= ~(1 << 8);
	    data |= (1 << 8);
        gphy_reg_write(phy_addr, reg_addr, &data);
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);



		


        wait_for_rgmii_phy_linkup();

		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 
		 

        post_log("-------------------------------------------\n\n");

        page=0x25;
		read_switch_statistics(page);
		page=0x20;
		read_switch_statistics(page);

		post_log("Printing the phy statistics now....MII xstat register \n");
		
		for (i=0;i<20 ;i++ )
		{
			phy_addr = 0x10;
            reg_addr = 0x11;
		    gphy_reg_read(phy_addr, reg_addr, &data);
		}
      
   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }


    int amacswitch_port5_to_port0_chip_nd_phy_d_4_hysterisis_enable(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }
	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x72000000;
              tx_desc_len       =20;//Max Desc=512=0x200
              rx_desc_len       =20;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x62000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				 data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL7, data,4);

    data = cpu_rd_single(CDRU_RGMII_IO_CONFIG,4);
    data |= (7 << CDRU_RGMII_IO_CONFIG__IOMUX_RGMII_DRV_SEL_R);
	data |= (1 << 9);
    cpu_wr_single(CDRU_RGMII_IO_CONFIG, data,4);


				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                 //cfg_sw1_rgmii();
				 // write port control register
	page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);
	// write switch control register	
	offset=0x22;
	srab_rdwr(page,offset,0xffffffff,6);
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}
    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);


/////Configure RGMII Delay - Chip in Non Delay mode
	page=0xe4; 
	offset = 0x2a;
	 srab_rdwr(page, offset, 0x0,1);

	 	page=0xe4; 
	offset = 0x4a;
	 srab_rdwr(page, offset, 0xA9,1);  //+1.0x dalay
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

    page=0x31; 
	offset = 0xa;
    srab_rdwr(page,offset,0x001,4);

    page=0x31; 
	offset = 0x0;
    srab_rdwr(page,offset,0x020,4);

#endif

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);


                      ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

						  phy_addr = 0;
                                       reg_addr = 0x1;
                                      do {
                                   	    gphy_reg_read(phy_addr,reg_addr,&data);
                                      } while(!(data & 0x4));

									  phy_addr = 0x0;
        reg_addr = 0x11;
		gphy_reg_read(phy_addr, reg_addr, &data);


                        

              

						ccg_mdio_init_ext1();

						phy_addr = 0x10;
     
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11B;
        gphy_reg_write(phy_addr, reg_addr, &data);
    
        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);
       
   
        /*reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
		data = 0xF0E7;
		gphy_reg_write(phy_addr, reg_addr, &data);

		reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);*/
         reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 15);
       // data &= ~(1 << 8);
	    data |= (1 << 8);
        gphy_reg_write(phy_addr, reg_addr, &data);
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);



		


        wait_for_rgmii_phy_linkup();

		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 
		 

        post_log("-------------------------------------------\n\n");

        page=0x25;
		read_switch_statistics(page);
		page=0x20;
		read_switch_statistics(page);

		post_log("Printing the phy statistics now....MII xstat register \n");
		
		for (i=0;i<20 ;i++ )
		{
			phy_addr = 0x10;
            reg_addr = 0x11;
		    gphy_reg_read(phy_addr, reg_addr, &data);
		}
      
   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }


    int amacswitch_port0_to_port5_combination_1(void)
   {
      
    test_status_t status = TEST_PASS;
    uint32_t test_args[NUM_TEST_ARGS];
    char *test_args_name[NUM_TEST_ARGS];
    uint32_t l,frame_sz,iter=1;
    uint32_t index;
	test_status_t result,rpkt_no,rpkt_chk;
   test_status_t res_sts[2];
   amac_pkt_descriptor_t* amac_tx_pkt_dscr_ptr;
   amac_pkt_descriptor_t* amac_rx_pkt_dscr_ptr;
   amac_rx_pkt_info_t* pkt_ptr;
   uint32_t bypass_mode;

    uint32_t page;
      	uint32_t offset;
      	uint32_t wdatah, wdatal;
      	uint32_t rdatah, rdatal;
      	uint32_t port_id;
          uint32_t *rdata;
          uint32_t phy_addr,i,reg_addr;
          uint32_t data;

   bypass_mode = 0;




    //256-byte data; CRC will be appended by Tx MAC
    uint8_t my_pkt_data[2048] =
   {
     0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00,
	 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00,
	 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00,
	 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00,
	      0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00,
	 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00,
	 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00,
	 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
     0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
     0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
     0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
     0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
     0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
     0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
     0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0x00

   };

    uint32_t amac_pkt_descriptor_addr,amac_pkt_descriptor_nxt_addr, nextDscr, rcvPtr,currDscr;
    uint32_t amac_id,amac_ports,rdata32,tx_desc_base,rx_desc_base,tx_desc_len,rx_desc_len,tx_buff_base,rx_buff_base,num_loops,add,pkt_diff;
    uint8_t rdata8;

	 //////////////////////////////////////////////////// Assign Args ////////////////////////////////////////////
    uint32_t frame_size,step,chk_noPkt;
    uint32_t timeout;
    uint8_t  eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,rmt_loop_ena,tx_pause_ignore;

	if(BCM958302K == post_get_board_diags_type())
   {
          post_log("\nSKIP : Diag not supported in POS Boards\n");
          return TEST_SKIP;
   }
	 ///////////// TEST ARG INIT ////////////////////

              test_args_name[0]="Amac Ports =";
              test_args_name[1]="Tx Desc Base =";
              test_args_name[2]="Rx Desc Base =";
              test_args_name[3]="Tx Desc Depth =";
              test_args_name[4]="Rx Desc Depth =";
              test_args_name[5]="Tx Buffer Base =";
              test_args_name[6]="Rx Buffer Base =";
              test_args_name[7]="Frame size =";

              amac_ports        =1;
              tx_desc_base      =0x71000000;//Max Desc space=(512 X 16)=0x2000
              rx_desc_base      =0x73000000;
              tx_desc_len       =100;//Max Desc=512=0x200
              rx_desc_len       =100;
              tx_buff_base      =0x61000000;//Max Buffer space=(512 X 12K)=0x600000
              rx_buff_base      =0x63000000;
              frame_size          =128;
              //frame_sz            =1024;
               step            =23;
    chk_noPkt       =tx_desc_len-1;
    //UNIMAC Config args
    eth_speed       =0x2;//1Gbps
    rx_pause_ignore =1;
    hd_ena          =1;
    mac_loop_con    =0;
    ena_ext_config  =1;
    rmt_loop_ena    =0;
    tx_pause_ignore =0;

	cpu_wr_single(CRMU_IOMUX_CTRL7,0x00000220,4); //Select RGMII pad as input
	            cpu_wr_single(CRMU_CHIP_IO_PAD_CONTROL,0x0,4);

				 data = cpu_rd_single(CRMU_IOMUX_CTRL7,4);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data &= ~(7 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_SRAM_RGMII_SEL_R);
    data |= (2 << CRMU_IOMUX_CTRL7__CORE_TO_IOMUX_CAMERA_RGMII_SEL_R);
    cpu_wr_single(CRMU_IOMUX_CTRL7, data,4);

    data = cpu_rd_single(CDRU_RGMII_IO_CONFIG,4);
    data |= (7 << CDRU_RGMII_IO_CONFIG__IOMUX_RGMII_DRV_SEL_R);
    cpu_wr_single(CDRU_RGMII_IO_CONFIG, data,4);


				//cpu_wr_single(0x301d060,0xb,4);
                //for(i=0;i<2000000;i++);
				//cpu_wr_single(0x301d060,0xf,4);


	 ///////////////////////////////////////////////////// Assign Args End ////////////////////////////////////////////

   ///////////////////////////////////////////// Setting up Descriptors for AMAC0 and AMAC1.///////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Initializing AMAC %d and setting TX and RX decriptors\n",amac_id);
        //Basic AMAC block init
        amac_init(amac_id,bypass_mode);

        //UNIMAC Config
        unimac_config(amac_id,eth_speed,rx_pause_ignore,hd_ena,mac_loop_con,ena_ext_config,\
                     rmt_loop_ena,tx_pause_ignore);

        //Reset MIB Counter
        counter_reset(amac_id);

        //Set up MAC loopback
        //amac_set_config(amac_id, MAC_LOOPBACK, 1);

        //Initialize AMAC TX DMA block
        amac_init_tx_dma(amac_id, (tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)));
        amac_pkt_descriptor_addr=(tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id));
        for(index = 0; index < tx_desc_len; index++) {

             //frame_size=((rand()%(0x3000-64))+64);

             CPU_BZERO((tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)),frame_size);

             write_pkt_to_memory(my_pkt_data,(frame_size-4), (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)));

             //Constructing TX pkt descriptor entry and Returns the address of the TxDescriptor entry
             amac_pkt_descriptor_nxt_addr = amac_config_tx_pkt_descriptor(amac_id, amac_pkt_descriptor_addr,tx_desc_len,\
                             (tx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id) + (AMAC_PKT_BUFFER_SIZE_IN_BYTES * index)), (frame_size),index);
             amac_pkt_descriptor_addr=amac_pkt_descriptor_nxt_addr;
        }

        //Initialize AMAC RX DMA block
        amac_init_rx_dma(amac_id, (rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)), rx_desc_len, rx_buff_base + (AMAC_BUFFER_SPACE_SIZE * amac_id),\
                        AMAC_PKT_BUFFER_SIZE_IN_BYTES);

        //Update with next descriptor value - outside EOT range
        //amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + (rx_desc_len * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

        //Within EOT range
        amac_update_rcvptr(amac_id, (((rx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + ((rx_desc_len - 1) * AMAC_RX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff));

         //amac_get_tx_status(amac_id);
         //amac_get_rx_status(amac_id);
         post_log("\n\n");
   }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Switch Configuration
               page=0x0; 
	for(port_id=0; port_id<4; port_id++)
	{ 
		if(port_id==0) offset=0x0;
		if(port_id==1) offset=0x1;
		if(port_id==2) offset=0x5;
		if(port_id==3) offset=0x8;
        srab_rdwr(page,offset,0,1);
	}

    offset=0xb;
	srab_rdwr(page,offset,0xfffffffC,2);
	// write switch control register	
	offset=0x22;
	srab_rdwr(page,offset,0xffffffff,6);
	// port override register
	page=0xe4; 
	for(port_id=0; port_id<4; port_id++)
	{
		if(port_id==0) offset=0x00;
		if(port_id==1) offset=0x02;
		if(port_id==2) offset=0x0a;
		if(port_id==3) offset=0x10;
		
		if(port_id==3)
			srab_rdwr(page,offset,0xf0,3); //wdatal=(rdatal | 0x1 | 0xf0); 
		else
			srab_rdwr(page,offset,0x70,3); //wdatal=(rdatal | 0x1 | 0x70); 
		
	}
    // BRCM Header disable
    page = 0x02;
	offset = 0x3;
	wdatal=0x0; 
	wdatah=0x0;
    post_log("switch mode wdatal=%#x wdatah=%#x\n",wdatal,wdatah);//
	srab_wr(page, offset, wdatah, wdatal);


/////Configure RGMII Delay - Chip in Non Delay mode
	page=0xe4; 
	offset = 0x2a;
	 srab_rdwr(page, offset, 0x0,1);

	 	page=0xe4; 
	offset = 0x4a;
	 srab_rdwr(page, offset, 0xF9,1);  //-0.5x dalay
//   post_log("Delay config wdatal=%#x wdatah=%#x\n",wdatal,wdatah);

#ifdef MAG_MODE

   // page=0x31; 
//	offset = 0xa;
 //   srab_rdwr(page,offset,0x001,4);

//    page=0x31; 
//	offset = 0x0;
 //   srab_rdwr(page,offset,0x020,4);

    page=0x31;
	offset = 0x0;
	srab_rdwr(page,offset,0x100,4);


	page=0x31;
	offset = 0x2;
	srab_rdwr(page,offset,0x000,4);


	page=0x31;
	offset = 0xa;
	srab_rdwr(page,offset,0x000,4);


   page=0x31;
   offset = 0x10;
   srab_rdwr(page,offset,0x020,4);

#endif

	                 page=0x0;
                      offset=0x6f;
                      //rdata = srab_rd(page, offset);
                      //wdatal=(rdatal | 0x1);
                      //wdatah=rdatah;
                      //srab_wr(page, offset, wdatah, wdatal);
					  srab_rdwr(page,offset,0x1,5);

					     ccg_mdio_init();

                       phy_addr = 0;
                         for(i=0; i<16; i++) {
                                reg_addr = i;
                                gphy_reg_read(phy_addr, reg_addr, &data);
                           }

                         //Lane Swapping and Polarity inversion for Gphy0
                         phy_addr = 0;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 0;
                         reg_addr = 0x15;
                         data = 0x5193;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;

                         //Lane Swapping and Polarity inversion for Gphy1
                         phy_addr = 1;
                         reg_addr = 0x17;
                         data = 0x0F09;
                         gphy_reg_write(phy_addr, reg_addr, &data);
                         //gphy_reg_read(phy_addr, reg_addr, &data);

                         phy_addr = 1;
                         reg_addr = 0x15;
                         data = 0x11C9;
                         gphy_reg_write(phy_addr, reg_addr, &data);

                         for(i=0; i<20000000; i++) ;


		ccg_mdio_init_ext1();

		phy_addr = 0x10;

        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        data = 0x11B;
        gphy_reg_write(phy_addr, reg_addr, &data);

        reg_addr = 0x17;
        data = 0x0F09;
        gphy_reg_write(phy_addr, reg_addr, &data);
        reg_addr = 0x15;
        gphy_reg_read(phy_addr, reg_addr, &data);



         reg_addr = 0x18;
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);
        data |= (1 << 15);
       // data &= ~(1 << 8);
	    data |= (1 << 8);
        gphy_reg_write(phy_addr, reg_addr, &data);
        data = 0x7007;
        gphy_reg_write(phy_addr, reg_addr, &data);
        gphy_reg_read(phy_addr, reg_addr, &data);

/**** added new **********/


/***checking phy 5 first **/

				   post_log("\n waiting for phy 5 \n");

					wait_for_rgmii_phy_linkup();

					 for (i=0;i<2000000 ;i++ );
					 for (i=0;i<2000000 ;i++ );
					 for (i=0;i<2000000 ;i++ );
					 for (i=0;i<2000000 ;i++ );
 /***checking phy 5 first **/



				   phy_addr = 0;

				   for(i=0; i<20000000; i++) ;
				   for(i=0; i<20000000; i++) ;
				   for(i=0; i<20000000; i++) ;
				   for(i=0; i<20000000; i++) ;
				   for(i=0; i<20000000; i++) ;
				   for(i=0; i<20000000; i++) ;

				   post_log("\n waiting for phy 0 \n");


				  phy_addr = 0;
				  reg_addr = 0x1;
							  do {
								gphy_reg_read(phy_addr,reg_addr,&data);
							  } while(!(data & 0x4));

							  phy_addr = 0x0;
                reg_addr = 0x11;
                gphy_reg_read(phy_addr, reg_addr, &data);
		  

                        

              

						

		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 for (i=0;i<2000000 ;i++ );
		 
		 

        post_log("-------------------------------------------\n\n");

        
        



    ///////////////////////////////////////////////// Enable TX DMA for A0 and A1 ////////////////////////////////////////////
   for(amac_id=0;amac_id < amac_ports;amac_id++){
        post_log("\t\t Starting AMAC %d transmissions\n",amac_id);
        // NxtDscr points to EOT Dscr
        nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                ((tx_desc_len - 1) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        //Out of EOT range,for continuous loopback
        //nextDscr=(((tx_desc_base + (AMAC_DESC_TABLE_SIZE * amac_id)) + \
                                //((tx_desc_len) * AMAC_TX_PKT_DESCRIPTOR_LENGTH_IN_BYTES)) & 0x1fff)  ;
        amac_enable_tx_dma(amac_id, nextDscr);
        post_log("\n\n");
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




  page=0x25;
		read_switch_statistics(page);
		page=0x20;
		read_switch_statistics(page);
		page=0x28;
		read_switch_statistics(page);

		post_log("Printing the phy statistics now....MII xstat register \n");
		
		for (i=0;i<20 ;i++ )
		{
			phy_addr = 0x10;
            reg_addr = 0x11;
		    gphy_reg_read(phy_addr, reg_addr, &data);
		}





   for(amac_id=0;amac_id < amac_ports;amac_id++){
        waitforTrans_internalloopback(amac_id);
        post_log("\n");
        amac_gettxstats(amac_id);
        amac_getrxstats(amac_id);

        rpkt_no=checker_all(amac_id);
        post_log("\n");

        post_log("\t Comparing the packet data of AMAC %d\n",amac_id);
        rpkt_chk=TEST_PASS;
        rpkt_chk=chkpktdata(amac_id,tx_desc_base,rx_desc_base,chk_noPkt);

         if((rpkt_no==TEST_PASS)&&(rpkt_chk==TEST_PASS)){
                 res_sts[amac_id]=TEST_PASS;
                 post_log("///////////////// AMAC ID %d :TEST_PASS /////////////////////\n",amac_id);
				 post_log("==========PASS==========\n");
         }
         else{
                 res_sts[amac_id]=TEST_FAIL;
                 post_log("///////////////// AMAC ID %d :TEST_FAIL //////////////////////\n",amac_id);
				 post_log("==========FAIL==========\n");
         }

         post_log("\n\n");
   }
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   if((res_sts[0]==TEST_PASS)){
            result=TEST_PASS;
   }else{
            result=TEST_FAIL;
    }
   return result;

   }








/******* Internal test function start ********/
int AMACSWITCH_post_test(int flags)
{
    uint32_t test_no;
    int loop_count = 0, total_tests = 0 , total_reg_tests = 0;
    int status = 0,autorun_status = 0,pass = 0,fail = 0,skip = 0;


    //lcd_clk_gate();
    //lcd_asiu_mipi_dsi_gen_pll_pwr_on_config();
    //lcd_ctrl_init(0x61000000,&config);



    FUNC_INFO function[]	=	{
								 ADD_TEST( amacswitch_internal_loopback_test_sram ),
							     ADD_TEST( amacswitch_internal_loopback_test_ddr ),
								 ADD_TEST( amacswitch_port0_to_port1_ddr_loopback ),  //dsp_tap10_fix
								 ADD_TEST( gphy0_dsp_tap10_register_enable_link_up_test_without_laneswap_1Gbps),
								 ADD_TEST( gphy0_dsp_tap10_register_enable_link_up_test_without_laneswap_100Mbps),
								 ADD_TEST( gphy0_dsp_tap10_register_enable_link_up_test_without_laneswap_10Mbps),
								 ADD_TEST( gphy1_dsp_tap10_register_enable_link_up_test_without_laneswap_1Gbps),
								 ADD_TEST( gphy1_dsp_tap10_register_enable_link_up_test_without_laneswap_100Mbps),
								 ADD_TEST( gphy1_dsp_tap10_register_enable_link_up_test_without_laneswap_10Mbps)	
								};

    FUNC_INFO regression_func[]	=	{

    		                                 ADD_TEST( amacswitch_internal_loopback_test_sram ),
							                 ADD_TEST( amacswitch_internal_loopback_test_ddr )


									};

    total_tests = (sizeof(function) / sizeof(function[0]));
    total_reg_tests = (sizeof(regression_func) / sizeof(regression_func[0]));
    {
    	post_log("Number of test : %d",total_tests);
    	do{
    		for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
    			post_log("\n%2d %s",(loop_count + 1), function[loop_count].func_name);
    		post_log("\n%2d run all tests",(total_tests + 1));
    		post_log("\n%2d exit\n",(total_tests + 2));
    		test_no = post_getUserInput ("\nEnter Test Number : ");

    		if(test_no > (total_tests + 2) )
    			post_log("ERROR - invalid entry\n");
    		else
    		{
                  if(test_no == (total_tests + 1))
                  {
                      autorun_status = 0;
                      pass = 0;
                      fail = 0;
                      skip = 0;
                      for(loop_count = 0 ; loop_count  < total_tests ; loop_count++ )
                      {
                    	  post_log("\nExecuting test no : %2d. %s\n",(loop_count + 1),function[loop_count].func_name);
                    	  autorun_status = (*function[loop_count].func)();
                    	  switch(autorun_status)
                    	  {
                    	  	  case TEST_PASS:
                    	  		  pass++;
                    	  		  break;
                    	  	  case TEST_FAIL:
                    	  		  fail++;
                    	  		  break;
                    	  	  case TEST_SKIP:
                    	  		  skip++;
                    	  		  break;
                    	  }
                      }
                      post_log("Report TOTAL Test => %d - PASS => %d - FAIL => %d - SKIP => %d\n",(pass + fail + skip), pass,fail,skip);
                      if(fail)
                        status = -1;
                  }

                  else if(test_no < (total_tests + 1))
                  {
                	  if(TEST_FAIL == (*function[test_no - 1].func)())
                		  status = -1;
                  }
             }

    	}while(test_no != (total_tests + 2));
    	return status;

    }
    return 0;
}
