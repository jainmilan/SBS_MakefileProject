model mpcmodel.mod;
data systemdata.dat;
#data Temp_winter1.dat;
#data ocpdata.dat;
data test.dat;

let total_rooms:= 1;
let CoWI_CRT:=1 - ((alpha_o * tau) / C) ; 	# Coefficient of Weather Impact on Current Room Temperature
let CoWI_OAT:=(alpha_o * tau) / C; # Coefficient of Weather Impact on Outside Air Temperature
let CoHI_CRT:=(-1 * tau * density * specific_heat) / (C); # Coefficient of HVAC Impact on Current Room Temperature
let CoHI_SAT:=(tau * density * specific_heat) / (C); # Coefficient of HVAC Impact on Supply Air Temperature
let CoEI_OLEL:=(tau * Q_l) / C; # Coefficient of Equipment Impact on Occupant Lightening and Equipment Load
let CoOI_OHL:=(tau * Q_h) / C_; # Coefficient of Occupancy Impact on Occupant Heating Load
let CoSI_SCS:=(tau * Q_s) / C_; # Coefficient of SPOT Impact on SCS
let CoRC_CiRT:=1 - ((tau * alpha_r) / C_); # Coefficient of Region Coupling on Change in Room Temperature
let CoRC_CiR1T:=(tau * alpha_r) / (C - C_); # Coefficient of Region Coupling on Change in Region-1 Temperature
let Coefficient_Heating_Power:=(density * specific_heat) / HeatingEfficiency;
let Coefficient_Cooling_Power:=(density * specific_heat) / CoolingEfficiency;

let T_NoSPOT_Init[1]:= 21;
let Delta_T_SPOT_Init[1]:=0;
let duration := Nhorizon*6;
let SAT_Prev := 30;
for {time_slot in 1..7 }
{
	for {j in 1..duration}
	{
		let T_Outside[j] := tando[time_slot + j - 1, 2];
		for {k in 1..total_rooms}
		{
			let Occupancy[j, k] := tando[time_slot + j - 1, 3]; #Oc[time_slot + j - 1, 1];
		}
	}
	let Time_IH := (time_slot - 1) mod 6;
	
	if Time_IH = 0 then let buffer1:=0;
	if Time_IH > 0 then let buffer1:=1;
	if Time_IH = 0 then let buffer2:=0;
	if Time_IH > 0 then let buffer2:=Time_IH;
	
	#display duration;
	#display total_rooms;
	#display Nhorizon;
	#display Time_IH;
	
	#display buffer1;
	#display buffer2;
	display SAT_Prev;
	
	#display Coefficient_Heating_Power;
	#display Coefficient_Cooling_Power;
	#display Coefficient_Fan_Power;
	#display Coefficient_SPOT_Power;
	
	#display pmv_p1;
	#display pmv_p2;
	#display pmv_p3;
	#display pmv_p4;
	
	#display CoWI_CRT;			# Coefficient of Weather Impact on Current Room Temperature
	#display CoWI_OAT;			# Coefficient of Weather Impact on Outside Air Temperature
	#display CoHI_CRT;			# Coefficient of HVAC Impact on Current Room Temperature
	#display CoHI_SAT;			# Coefficient of HVAC Impact on Supply Air Temperature
	#display CoEI_OLEL;		# Coefficient of Equipment Impact on Occupant Lightening and Equipment Load
	#display CoOI_OHL;			# Coefficient of Occupancy Impact on Occupant Heating Load
	#display CoSI_SCS;			# Coefficient of SPOT Impact on SCS
	#display CoRC_CiRT;		# Coefficient of Region Coupling on Change in Room Temperature
	#display CoRC_CiR1T;		# Coefficient of Region Coupling on Change in Region-1 Temperature
	
	# Limits on Varibles
	#display SAT_ll;			# Supply Air Temperature Lower Limit
	#display SAT_ul;			# Supply Air Temperature Upper Limit
	#display SAV_ll;			# Supply Air Volume Lower Limit
	#display SAV_ul;			# Supply Air Volume Upper Limit
	#display PMV_ll;			# Personalized Mean Vote (PMV) Lower Limit
	#display PMV_ul;			# Personalized Mean Vote (PMV) Upper Limit
	#display T_ll;				# Room Temperature Due To HVAC Lower Limit
	#display T_ul;				# Room Temperature Due To HVAC Upper Limit
	#display T_SPOT_ll;		# Temperature in SPOT Region Lower Limit
	#display T_SPOT_ul;		# Temperature in SPOT Region Upper Limit
	#display T_NoSPOT_ll;		# Temperature in No SPOT Region Lower Limit
	#display T_NoSPOT_ul;		# Temperature in No SPOT Region Upper Limit
	
	#display T_Outside;
	#display Occupancy;
	#display T_NoSPOT_Init;
	#display Delta_T_SPOT_Init;
	
	#option presolve 0;
	
	let SPOT_Status[1, 1] := 2;
	display SPOT_Status;
	
	option solver minos;
	solve;

	let response := solve_result_num;
	display response;
	#print {i in 1.._ncons: _con[i].status="pre"}: _conname[i];
	#display T_Mixing_Unit[2];
	#display Ratio;
	#display T_NoSPOT[2];
	#display Delta_T_SPOT[2, 1];
	#display SPOT_Status;
	#display SAT[1];
	#display T_Outside[1];
	#display Occupancy;
	
	let heating_power := SAV[1] * Coefficient_Heating_Power * ( SAT[1] - T_Cooling_Unit[1] );
	let cooling_power := SAV[1] * Coefficient_Cooling_Power * ( T_Mixing_Unit[1] - T_Cooling_Unit[1] );
	let fan_power := Coefficient_Fan_Power * ( SAV[1] ^ 2 );
	let spot_power := sum {k in 1..total_rooms} (( Coefficient_SPOT_Power * SPOT_Status[1, k] ));  
	
	let power := heating_power + cooling_power + fan_power + spot_power;
	
	display power;
	
	let SAT_Prev := SAT[1];
	
	for {i in 1..total_rooms}
	{
		let T_NoSPOT_Init[i] := T_NoSPOT[2, i];
		let Delta_T_SPOT_Init[i] := Delta_T_SPOT[2, i];
	}
 }
