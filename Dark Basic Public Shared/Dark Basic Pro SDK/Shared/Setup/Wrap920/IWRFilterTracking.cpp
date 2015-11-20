//
// apply filtering algo. on continuous head tracking samples.
//  This algorythm is a very primitive running-sumation coupled with a noise detection
//  scheme,  to provide:
//		1: Fast responce when the user's orientation is in constant motion.
//      2: Smooth tracking when the user is focused at a more fixed orientation.
//
#include <math.h>
// As defined in IWRsdk.h
#define IWR_RAWTODEG				180.0f / 32768.0f

#define MAX_FILTER_SIZE				16
#define DEFAULT_FILTER_SIZE			8
#define DEFAULT_FILTER_ZONE			3.0f
#define MAX_FILTER_ZONE				8.0f
#define MIN_FILTER_ZONE				0.25f

float				Filtered_yaw[ MAX_FILTER_SIZE ];
float				Filtered_pitch[ MAX_FILTER_SIZE ];
float				Filtered_roll[ MAX_FILTER_SIZE ];
int					Max_Filter_entries = DEFAULT_FILTER_SIZE; 
int					g_Filter_index = 0;
int					Filter_Restart_Count = 1;
// The below settings; could be made adjustable via user input?
long				g_FilterSize= DEFAULT_FILTER_SIZE;
float				g_FilterZone= DEFAULT_FILTER_ZONE;

void	IWRFilterTracking( long *yaw, long *pitch, long *roll )
{
int		i, FilterCnt, Previous_Stored_index;
float	FilterSum_yaw = 0.0f, FilterSum_pitch = 0.0f, FilterSum_roll = 0.0f;
float	Current_yaw, Previous_yaw;
float	Current_pitch, Previous_pitch;
	Previous_Stored_index = g_Filter_index;
	g_Filter_index++;
	if( g_Filter_index >= MAX_FILTER_SIZE ) 
		g_Filter_index = 0;
	Filtered_yaw[ g_Filter_index ]	= (float)*yaw;
	Filtered_pitch[ g_Filter_index ]= (float)*pitch;
	Filtered_roll[ g_Filter_index ] = (float)*roll;
	// compute max filter entries based on noise in signal.
	// This should be StdDev of current set or other rms solution.
	Current_yaw    = (float)Filtered_yaw[ g_Filter_index ] * IWR_RAWTODEG;
	Previous_yaw   = (float)Filtered_yaw[ Previous_Stored_index ] * IWR_RAWTODEG;
	Current_pitch  = (float)Filtered_pitch[ g_Filter_index ] * IWR_RAWTODEG;
	Previous_pitch = (float)Filtered_pitch[ Previous_Stored_index ] * IWR_RAWTODEG;
	// If the filter noise goes above a threashold turn off filtering .
	if( fabs(Previous_yaw - Current_yaw) > g_FilterZone ||
		fabs(Previous_pitch - Current_pitch) > g_FilterZone ) {
		// Disable filtering when outside of quiet zone operation.
		Filter_Restart_Count = (g_FilterSize * 2);
		}
	// Determine when to enter into filter mode again.
	if( Filter_Restart_Count ) {
        Max_Filter_entries = 1;
		Filter_Restart_Count--;
		if( Filter_Restart_Count == 0 ) {
			// Fill up filter que with current reading.
			for( i=0; i < MAX_FILTER_SIZE; i++ ) {
				Filtered_yaw[ i ]  = (float)*yaw;
				Filtered_pitch[ i ]= (float)*pitch;
				Filtered_roll[ i ] = (float)*roll;
				}
			}
		}
	else {// Setup number of filter values to process.
		Max_Filter_entries = g_FilterSize;
		}
	// Simple running average. from most recent to front of que.
	for( i=g_Filter_index, FilterCnt=0; i; i-- ) {
		if( FilterCnt++ >= Max_Filter_entries ) 
			break;
		FilterSum_yaw   += Filtered_yaw[ i ];
		FilterSum_pitch += Filtered_pitch[ i ];
		FilterSum_roll  += Filtered_roll[ i ];
		}
	// Continue from the end of the que.
	for( i=MAX_FILTER_SIZE-1; i > g_Filter_index; i-- ) {
		if( FilterCnt++ >= Max_Filter_entries ) 
			break;
		FilterSum_yaw   += Filtered_yaw[ i ];
		FilterSum_pitch += Filtered_pitch[ i ];
		FilterSum_roll  += Filtered_roll[ i ];
		}
	*yaw	= (long)(FilterSum_yaw	/ (float)Max_Filter_entries);
	*pitch	= (long)(FilterSum_pitch/ (float)Max_Filter_entries);
	*roll	= (long)(FilterSum_roll / (float)Max_Filter_entries);
}
