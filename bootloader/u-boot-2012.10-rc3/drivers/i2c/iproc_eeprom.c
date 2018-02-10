
#include <common.h>
#include <command.h>
#include <environment.h>
#include <linux/stddef.h>

#include "iproc_i2c_regs.h"
#include "iproc_i2c_defs.h"
#include "iproc_i2c.h"
#include "errno.h"

DECLARE_GLOBAL_DATA_PTR;

fac_env_t *fac_env_ptr = NULL;

char * fac_env_name_spec = "EEPROM";

uchar fac_default_environment[] = {
#ifdef  CONFIG_DEFAULT_FACTORY_ENV
	CONFIG_DEFAULT_FACTORY_ENV
#endif
	"\0"
};


int eeprom_read(unsigned dev_addr, unsigned offset, uchar *buffer,
			unsigned cnt)
{

	if(0 != i2c_read(dev_addr, offset, 1, buffer, cnt))
		return 1;

	udelay(10000);
	return 0;
}

int eeprom_write(unsigned dev_addr, unsigned offset, uchar *buffer,
			unsigned cnt)
{
		if(i2c_write(dev_addr, offset, 1, buffer, cnt) != 0)
			return 1;

	udelay(10000);
	return 0;
}


static int eeprom_bus_read (unsigned dev_addr, unsigned offset, uchar *buffer,
				unsigned cnt)
{
	int rcode;

	rcode = eeprom_read (dev_addr, offset, buffer, cnt);

	return rcode;
}

static int eeprom_bus_write (unsigned dev_addr, unsigned offset, uchar *buffer,
				unsigned cnt)
{
	int rcode;
	rcode = eeprom_write (dev_addr, offset, buffer, cnt);
	
	return rcode;
}

uchar fac_env_get_char_spec (int index)
{
	uchar c;
	unsigned int off;
	off = CONFIG_PDT_ENV_OFFSET;

	eeprom_bus_read (CONFIG_SYS_DEF_EEPROM_ADDR,
		     off + index + offsetof(fac_env_t,data),
		     &c, 1);

	return (c);
}

uchar fac_env_get_char_memory (int index)
{
	if (gd->fac_env_valid) {
		return ( *((uchar *)(gd->fac_env_addr + index)) );
	} else {
		return ( fac_default_environment[index] );
	}
}
static uchar fac_env_get_char_init (int index)
{
	uchar c;

	/* if crc was bad, use the default environment */
	if (gd->fac_env_valid)
	{
		c = fac_env_get_char_spec(index);
	} else {
		c = fac_default_environment[index];
	}

	return (c);
}

uchar fac_env_get_char (int index)
{
	uchar c;

	/* if relocated to RAM */
	
	if (gd->flags & GD_FLG_RELOC)
		c = fac_env_get_char_memory(index);
	else
		c = fac_env_get_char_init(index);

	return (c);
}

int fac_envmatch (uchar *s1, int i2)
{

	while (*s1 == fac_env_get_char(i2++))
		if (*s1++ == '=')
			return(i2);
	if (*s1 == '\0' && fac_env_get_char(i2-1) == '=')
		return(i2);
	return(-1);
}

uchar *fac_env_get_addr (int index)
{
	if (gd->fac_env_valid) {
		return ( ((uchar *)(gd->fac_env_addr + index)) );
	} else {
		return (&fac_default_environment[index]);
	}
}

char *getfacenv (char *name)
{
	int i, nxt;

	for (i=0; fac_env_get_char(i) != '\0'; i=nxt+1) {
		int val;

		for (nxt=i; fac_env_get_char(nxt) != '\0'; ++nxt) {
			if (nxt >= CONFIG_ENV_SIZE) {
				return (NULL);
			}
		}
		if ((val=fac_envmatch((uchar *)name, i)) < 0)
			continue;
		return ((char *)fac_env_get_addr(val));
	}

	return (NULL);
}

void fac_env_relocate_spec (void)
{
	unsigned int off = CONFIG_PDT_ENV_OFFSET;
	eeprom_bus_read (CONFIG_SYS_I2C_EEPROM_ADDR,
		     off,
		     (uchar*)fac_env_ptr,
		     CONFIG_PDT_ENV_SIZE);
}

void fac_env_crc_update (void)
{
	fac_env_ptr->crc = crc32(0, fac_env_ptr->data, FAC_ENV_SIZE);
}

void fac_set_default_env(void)
{
	if (sizeof(fac_default_environment) > FAC_ENV_SIZE) {
		puts ("*** Error - default environment is too large\n\n");
		return;
	}

	memset(fac_env_ptr, 0, sizeof(fac_env_t));
	memcpy(fac_env_ptr->data, fac_default_environment,
	       sizeof(fac_default_environment));
	fac_env_crc_update ();
	gd->fac_env_valid = 1;
}

int fac_saveenv(void)
{
	int rc;
	unsigned int off = CONFIG_PDT_ENV_OFFSET;

	rc = eeprom_bus_write (CONFIG_SYS_I2C_EEPROM_ADDR,
			     off,
			     (uchar *)fac_env_ptr,
			     CONFIG_PDT_ENV_SIZE);


	return rc;
}

int fac_env_check(void)
{
	if(NULL == fac_env_ptr)
	{
		printf("fac_env_ptr malloc\n");
		fac_env_ptr = (fac_env_t *)malloc(sizeof(fac_env_t));
	}
	if (gd->fac_env_valid == 0) {
			fac_set_default_env();
			printf("ecc error,wait fpga reset cpu\n");
			mdelay(5000);
	}
	else {
		fac_env_relocate_spec ();
	}
	gd->fac_env_addr = (ulong)&(fac_env_ptr->data);

	/* load & check factory env into env */
	{
		char *pvalue;
		char *eth0mac;

	    /* set serial# ethaddr and MacExt if not yet defined */
	    pvalue = getfacenv("serial#");
	    if (pvalue == NULL) {
	        puts ("*** Error - serial No. is not set in factory setting \n\n");
	    }
	    
	    pvalue = getfacenv("1st_mac");
	    if (pvalue != NULL && pvalue[9] != 'X') {
			eth0mac = getenv("ethaddr");	
			if(strncmp(eth0mac, pvalue, 17))
			{
	        	setenv ("ethaddr", pvalue);
				puts("ethaddr update form factory info\n");
				saveenv();
			}
	    } else {
	    	puts ("*** Error - macaddress is not set in factory setting \n\n");
	    }

	    pvalue = getfacenv("hardware_version");
	    if (pvalue == NULL) {
	        puts ("*** Error - hardware_version. is not set in factory setting \n\n");
	    }

	    pvalue = getfacenv("hwcfg");
	    if (pvalue == NULL) {
	        puts ("*** Error - hwcfg. is not set in factory setting \n\n");
	    }
	}

}
int fac_env_init(void)
{
	ulong crc, len, new;
	unsigned off;
	uchar buf[64];

	/* read old CRC */
	eeprom_bus_read (CONFIG_SYS_I2C_EEPROM_ADDR,
		     CONFIG_PDT_ENV_OFFSET+offsetof(fac_env_t,crc),
		     (uchar *)&crc, sizeof(ulong));

	new = 0;
	len = FAC_ENV_SIZE;
	off = offsetof(fac_env_t,data);
	while (len > 0) {
		int n = (len > sizeof(buf)) ? sizeof(buf) : len;

		eeprom_bus_read (CONFIG_SYS_I2C_EEPROM_ADDR,
				CONFIG_PDT_ENV_OFFSET + off, buf, n);
		new = crc32 (new, buf, n);
		len -= n;
		off += n;
	}
	

	if (crc == new) {
		gd->fac_env_addr  = offsetof(fac_env_t,data);
		gd->fac_env_valid = 1;
	} else {
		gd->fac_env_addr  = 0;
		gd->fac_env_valid = 0;
	} 
	printf("oldcrc =%lx newcrc =%lx \n",  crc, new);

	return (0);
}



