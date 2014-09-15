#include "lpc_phy.h"

/** \brief DP83848 PHY status definitions */
#define DP8_REMOTEFAULT    (1 << 6)   /**< Remote fault */
#define DP8_FULLDUPLEX     (1 << 2)   /**< 1=full duplex */
#define DP8_SPEED10MBPS    (1 << 1)   /**< 1=10MBps speed */
#define DP8_VALID_LINK     (1 << 0)   /**< 1=Link active */


    // This function returns the current status of connection.
static bool get_link_status()
{
    u32_t tmp = lpc_mii_read_data();        
    return (tmp & DP8_VALID_LINK) ? true : false;
}

    // This function returns the status of transmission.
static char* get_transmission_status()
{
    u32_t tmp = lpc_mii_read_data();
    if(tmp & DP8_FULLDUPLEX)
    { 
        return "FULL DUPLEX"; 
    }else
    {        
        return "HALF DUPLEX";
    }
}

    // This function returns the speed of the connection.
static int get_connection_speed()
{
    u32_t tmp = lpc_mii_read_data();
    return (tmp & DP8_SPEED10MBPS) ? 10 : 100;
}

    // This function returns the current value in the MII data register.
static u32_t mii_read_data()
{
    return lpc_mii_read_data();  // 16-bit MRDD - address 0x2008 4030                             
}