#for no common room change the printf Pm and w values
reset;
model mpc2zone.mod;
data sysdata.dat;
data inp_value.dat;
data temperature.dat;
data occupancy.dat;
#data var_value.dat;
data Pinvalue.dat;
data delPvalue.dat;
data Uvalue.dat;
let a0:=1-(tau*alp0/C1);
let a1:=-tau*rs/C1;
let a2:=1-(tau*alp1/C2);
let b1:=tau*rs/C1;
let d1:=tau*alp0/C1;
let d2:=tau/C1;
let d3:=tau*alp1/(C1-C2);
let d4:=tau/C2;
let b2:=tau/C2;

let N:=4;
let T:=6*N;
if l2=0 then let bf1:=1;
if l2>0 then let bf1:=0;
if l2=0 then let bf3:=1;
if l2>0 then let bf3:=l2;
option presolve_eps 1e-06;
option solver snopt;

#option solver baron;
#option baron_options 'lsolver=snopt maxtime=10000 barstats epsa=1e-2 prfreq=100 prloc=1 sumfile=summary';
solve;
let power_hvac:=(v[1]*th1*(u[1]-Tc[1])) +(v[1]*th2*(Tia[1]-Tc[1]))+ (th3*v[1]^2);
let power_spot:= sum{k in 1..n1}(th4*w[k,1]) + sum{k in 1..n1}(th5*va[k,1]);  
let power_total:= power_hvac+power_spot;   
    
printf "\n %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",solve_result_num, l1,l2, energy, w[1,1], w[2,1],w[3,1], w[4,1], w[5,1], P[1,2], P[2,2],P[3,2], P[4,2], P[5,2], u[1],r[1],v[1],Tia[1],Pm[1,2],Pm[2,2],Pm[3,2],Pm[4,2],Pm[5,2],
power_total, delP[1,2], delP[2,2],delP[3,2], delP[4,2],delP[5,2], power_hvac, power_spot,va[1,1], va[2,1],va[3,1], va[4,1],va[5,1]>>out_Nguess.dat;
 
#printf "\n %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f ",solve_result_num, l1,l2, energy, 
#w[1,1], w[2,1],w[3,1], w[4,1], w[5,1], P[1,2], P[2,2],P[3,2], P[4,2], P[5,2], u[1],r[1],v[1],Tia[1],Pm[1,2],Pm[2,2],Pm[3,2],Pm[4,2],Pm[5,2],bf2, delP[1,2], delP[2,2],delP[3,2], delP[4,2],delP[5,2]  >>out_Nguess1.dat;

 
    
    



