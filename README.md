# SBS_MakefileProject 

**Introduction:** Simple Building Simulator is a tool to quickly implement thermal models to understand impact of various factors such as weather conditions, activities of occupants, etc on  the room temperature. It can approximately simulate the behavior of a building having multiple HVAC zones, depending upon the thermal model. Aim of this project is to develop a tool which is quick and easy to handle for computer scientists working in space of HVAC optimization across residential and commercial buildings. 

**Run On Linux:**
- Clone the Repository: `git clone https://github.com/milanjain81/SBS_MakefileProject.git`
- Install AMPL (if installed skip this step):
  - 
- Install AMPL API (if installed skip this step):
  - 
- Link binaries of AMPL API
  - Type `sudo nano /etc/ld.so.conf` on your terminal and press `Enter`
  - Write path of directory having AMPL API libraries: `AMPL_Folder/AMPL_API_Folder/lib`
  - Press `Ctrl+O` and then `Enter`
  - Type `sudo ldconfig`
- `make clean`
- `make all`
- `./SimpleBuildingSimulator`
