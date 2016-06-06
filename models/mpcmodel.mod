# Basic Parameters
param duration;
param total_rooms;

# Paramters for objective function
param Coefficient_Heating_Power;
param Coefficient_Cooling_Power;
param Coefficient_Fan_Power;
param Coefficient_SPOT_Power; ## Need to be confirmed (Different in Rachel's Code)
## param theta4;
## param theta5;

# Coefficient Values
param CoWI_CRT;			# Coefficient of Weather Impact on Current Room Temperature
param CoWI_OAT;			# Coefficient of Weather Impact on Outside Air Temperature
param CoHI_CRT;			# Coefficient of HVAC Impact on Current Room Temperature
param CoHI_SAT;			# Coefficient of HVAC Impact on Supply Air Temperature
param CoEI_OLEL;		# Coefficient of Equipment Impact on Occupant Lightening and Equipment Load
param CoOI_OHL;			# Coefficient of Occupancy Impact on Occupant Heating Load
param CoSI_SCS;			# Coefficient of SPOT Impact on SCS
param CoRC_CiRT;		# Coefficient of Region Coupling on Change in Room Temperature
param CoRC_CiR1T;		# Coefficient of Region Coupling on Change in Region-1 Temperature

# PMV Parameters
param pmv_p1;
param pmv_p2;
param pmv_p3;
param pmv_p4;

# Limits on Varibles
param SAT_ll;			# Supply Air Temperature Lower Limit
param SAT_ul;			# Supply Air Temperature Upper Limit
param SAV_ll;			# Supply Air Volume Lower Limit
param SAV_ul;			# Supply Air Volume Upper Limit
## param volim1;
param PMV_ll;			# Personalized Mean Vote (PMV) Lower Limit
param PMV_ul;			# Personalized Mean Vote (PMV) Upper Limit
## param betalim11;
## param betalim12;
param T_ll;				# Room Temperature Due To HVAC Lower Limit
param T_ul;				# Room Temperature Due To HVAC Upper Limit
param T_SPOT_ll;		# Temperature in SPOT Region Lower Limit
param T_SPOT_ul;		# Temperature in SPOT Region Upper Limit
param T_NoSPOT_ll;		# Temperature in No SPOT Region Lower Limit
param T_NoSPOT_ul;		# Temperature in No SPOT Region Upper Limit

# Input Data
param T_Outside {1..duration};						# Outside Temperature
param Occupancy {1..duration, 1..total_rooms};		# Occupancy in the Room
param T_NoSPOT_Init {1..total_rooms}; 				# Initial Temperature in NoSPOT Region
param Delta_T_SPOT_Init {1..total_rooms}; 			# Initial Temperature Change in SPOT Region

# Variables
var T {1..duration, 1..total_rooms} >= 0;					# Room Temperature
var Delta_T_SPOT {1..duration+1, 1..total_rooms} >= 0;		# Change in Temperature of SPOT Region
var T_SPOT {1..duration, 1..total_rooms} >= 0;				# Temperature in SPOT Region
var Delta_T_NoSPOT {1..duration+1, 1..total_rooms} >= 0;	# Change in Temperature of No SPOT Region
var T_NoSPOT {1..duration+1, 1..total_rooms} >= 0;			# Temperature in No-SPOT Region
var T_Mixing_Unit {1..duration} >= 0;						# Temperature from Mixing Unit
var T_Cooling_Unit {1..duration} >= 0;						# Temperature from Cooling Unit
var SAT {1..duration} >= 0 ;								# Supply Air Temperature
var SAV {1..duration} >= 0 ;								# Supply Air Volume
var SPOT_Status {1..duration, 1..total_rooms} >= 0;			# SPOT State
var Ratio {1..duration} >= 0;								# Mixing Ratio
var PMV {2..duration+1, 1..total_rooms};					# PMV
var Fan_Speed {1..duration, 1..total_rooms} >= 0;			# Fan Speed

# Objective
minimize total_power : sum {t in 1..duration} (
							( SAV[t] * Coefficient_Heating_Power * ( SAT[t] - T_Cooling_Unit[t] ) ) + 
							( SAV[t] * Coefficient_Cooling_Power * ( T_Mixing_Unit[t] - T_Cooling_Unit[t] ) ) + 
							( Coefficient_Fan_Power * ( SAV[t] ^ 2 ) )
						) + 
						sum {t in 1..duration, k in 1..total_rooms} (
							( Coefficient_SPOT_Power * SPOT_Status[t, k] )
						);

# Constraints
subject to Initialize_Delta_Temperature_In_SPOT_Region {k in 1..total_rooms}: Delta_T_SPOT[1, k] = Delta_T_SPOT_Init[k];
subject to Delta_Temperature_In_SPOT_Region {t in 1..duration, k in 1..total_rooms}:
		Delta_T_SPOT[t+1, k] = ( CoRC_CiRT * Delta_T_SPOT[t, k] ) + ( CoSI_SCS * SPOT_Status[t, k] ) + 
						( CoOI_OHL * Occupancy[t, k] );

subject to Initialize_Temperature_In_NoSPOT_Region {k in 1..total_rooms}: T_NoSPOT[k, 1] = T_NoSPOT_Init[k];
subject to Temperature_In_NoSPOT_Region {t in 1..duration, k in 1..total_rooms}: 
		T_NoSPOT[t+1, k] = ( CoWI_CRT * T_NoSPOT[t, k] ) + ( CoWI_OAT * T_Outside[t] ) + 
						( CoHI_CRT * T_NoSPOT[t, k] * ( SAV[t] / total_rooms ) ) + 
						( CoHI_SAT * SAT[t] * ( SAV[t] / total_rooms ) ) + 
						( CoEI_OLEL * Occupancy[t, k] ) + 
						( CoRC_CiR1T * Delta_T_SPOT[t, k] );

subject to Mixed_Air_Temperature {t in 1..duration}:
		T_Mixing_Unit[t] = ( Ratio[t] * ( sum{k in 1..total_rooms} T_SPOT[t, k] ) / total_rooms  ) + 
							( 1 - Ratio[t] ) * ( T_Outside[t] );

subject to PMV_Constraint {t in 2..duration+1, k in 1..total_rooms}:
		PMV[t, k] = Occupancy[t-1, k] * ( pmv_p1 * ( T_NoSPOT[t, k] + Delta_T_SPOT[t, k] - 
		CoRC_CiR1T * Delta_T_SPOT[t-1, k] ) + ( pmv_p3 * Fan_Speed[t-1, k] * Fan_Speed[t-1, k] ) - 
		( pmv_p2 * Fan_Speed[t-1, k] ) - pmv_p4);

subject to PMV_lower_limit {t in 2..duration+1, k in 1..total_rooms}: PMV[t, k] >= PMV_ll;
subject to PMV_upper_limit {t in 2..duration+1, k in 1..total_rooms}: PMV[t, k] <= PMV_ul;

subject to T_NoSPOT_lower_limit {t in 2..duration+1, k in 1..total_rooms}: T_NoSPOT[t, k] >= T_NoSPOT_ll;
subject to T_NoSPOT_upper_limit {t in 2..duration+1, k in 1..total_rooms}: T_NoSPOT[t, k] <= T_NoSPOT_ul;

subject to SAV_lower_limit {t in 1..duration}: SAV[t] >= SAV_ll;
subject to SAV_upper_limit {t in 1..duration}: SAV[t] <= SAV_ul;

subject to SAT_lower_limit {t in 1..duration}: SAT[t] >= SAT_ll;
subject to SAT_upper_limit {t in 1..duration}: SAT[t] <= SAT_ul;

subject to Ratio_lower_limit {t in 1..duration}: Ratio[t] <= 0.8;

subject to T_Cooling_Unit_lower_limit_1 {t in 1..duration}: T_Cooling_Unit[t] <= SAT[t];
subject to T_Cooling_Unit_lower_limit_2 {t in 1..duration} : T_Cooling_Unit[t] <= T_Mixing_Unit[t];

subject to SPOT_Status_Constraint {t in 1..duration, k in 1..total_rooms}: SPOT_Status[t, k] <= Occupancy[t, k];

subject to Fan_Speed_Eq_Const {t in 1..duration, k in 1..total_rooms} : Fan_Speed[t, k] = 0;

##### Yet To Implement - Need Help
#subject to pmvlt3 {t in 2..T+1,k in  n1+1..n1+n2} :O[6*(l1-1)+l2+t-1,k]*P[k,t] >=O[6*(l1-1)+l2+t-1,k]*betalim11;
#subject to pmvlt4 {t in 2..T+1,k in n1+1..n1+n2} :O[6*(l1-1)+l2+t-1,k]*P[k,t] <=O[6*(l1-1)+l2+t-1,k]*betalim12;

#subject to ipcons1 {i in 0..(N-1),j in 1..5}:bf1*u[6*i+1]=bf1*u[6*i+j+1];
#subject to ipcons2 {j in 0..(5-l2)}:l2*u[1]=l2*u[1+j];
#subject to ipcons3 {i in 0..(N-2), j in 1..5}:l2*u[6*(i+1)-l2+1]=l2*u[6*(i+1)-l2+j+1];
#subject to ipcons4 {i in (N-1)..(N-1),j in 0..(bf3-1)}:l2*u[6*(i+1)-bf3+1]=l2*u[6*(i+1)-bf3+1+j];
#subject to ipcons5 :l2*u[1]=l2*U;

#subject to voclt {t in 1..T} : (sum{k in I}O[6*(l1-1)+l2+t,k])*v[t] >= (sum{k in I}O[6*(l1-1)+l2+t,k])*volim1;
