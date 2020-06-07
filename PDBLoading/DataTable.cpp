#include "DataTable.h"
//#include "Atoms.h"

DataTables::DataTables(void)
{
	create_CPK_ColourList();
	create_VanDerWaals_AtomicRadii();
}

DataTables::~DataTables(void)
{
}
/**************************************************************************
Here is the table of data the Van der Waals Radii used in the program comes
from. It is taken from:

A. Bondi (1964) "van der Waals Volumes and Radii" J.Phys.Chem. 68, 441-451

:::::::::::::::::::::::::::::::::::::::::::::::::::::
	Ag  1.72      Ar  1.88     As  1.85     Au  1.66
	Br  1.85      C   1.70     Cd  1.58     Cl  1.75
	Cu  1.40      F   1.47     Ga  1.87     H   1.20
	He  1.40      Hg  1.55     I   1.98     In  1.93
	K   2.75      Kr  2.02     Li  1.82     Mg  1.73
	N   1.55      Na  2.27     Ne  1.54     Ni  1.63
	O   1.52      P   1.80     Pb  2.02     Pd  1.63
	Pt  1.72      S   1.80     Se  1.90     Si  2.10
	Sn  2.17      Te  2.06     Tl  1.96     U  1.86
	Xe  2.16      Zn  1.39
:::::::::::::::::::::::::::::::::::::::::::::::::::::
***************************************************************************/
void DataTables::create_VanDerWaals_AtomicRadii()
{
	//cout << "Orig rad: " << SuperAtom::origHapticRadius << endl;

	//VDW_List["AG"] = (1.72f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["AR"] = (1.88f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["AS"] = (1.85f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["AU"] = (1.66f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["BR"] = (1.85f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["C"]  = (1.70f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["CD"] = (1.58f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["CL"] = (1.75f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["CU"] = (1.40f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["F"]  = (1.47f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["FE"]  = (1.47f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["GA"] = (1.87f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["H"]  = (1.20f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["HE"] = (1.40f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["HG"] = (1.55f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["I"]  = (1.98f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["IN"] = (1.93f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["K"]  = (2.75f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["KR"] = (2.02f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["LI"] = (1.82f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["MG"] = (1.73f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["N"]  = (1.55f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["NA"] = (2.27f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["NE"] = (1.54f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["NI"] = (1.63f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["O"]  = (1.52f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["P"]  = (1.80f*INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["PB"] = (2.02f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["PD"] = (1.63f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["PT"] = (1.72f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["S"]  = (1.80f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["SE"] = (1.90f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["SI"] = (2.10f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["SN"] = (2.17f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["TE"] = (2.06f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["TL"] = (1.96f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["U"]  = (1.86f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["XE"] = (2.16f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);
	//VDW_List["ZN"] = (1.39f *INITIAL_SCALE)+(SuperAtom::origHapticRadius);

	VDW_List["AG"] = (1.72f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["AR"] = (1.88f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["AS"] = (1.85f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["AU"] = (1.66f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["BR"] = (1.85f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["C"]  = (1.70f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["CD"] = (1.58f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["CL"] = (1.75f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["CU"] = (1.40f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["F"]  = (1.47f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["FE"]  = (1.47f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["GA"] = (1.87f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["H"]  = (1.20f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["HE"] = (1.40f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["HG"] = (1.55f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["I"]  = (1.98f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["IN"] = (1.93f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["K"]  = (2.75f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["KR"] = (2.02f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["LI"] = (1.82f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["MG"] = (1.73f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["N"]  = (1.55f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["NA"] = (2.27f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["NE"] = (1.54f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["NI"] = (1.63f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["O"]  = (1.52f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["P"]  = (1.80f*INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["PB"] = (2.02f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["PD"] = (1.63f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["PT"] = (1.72f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["S"]  = (1.80f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["SE"] = (1.90f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["SI"] = (2.10f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["SN"] = (2.17f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["TE"] = (2.06f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["TL"] = (1.96f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["U"]  = (1.86f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["XE"] = (2.16f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
	VDW_List["ZN"] = (1.39f *INITIAL_SCALE);//+(SuperAtom::origHapticRadius);
}

void DataTables::create_CPK_ColourList()
{

	float light_grey[] = {200.0f/255.0f, 200.0f/255.0f, 200.0f/255.0f};
	std::vector<float> light_greyVEC(light_grey, light_grey + 3);
	float red[3] = {240.0f/255.0f, 0.0f, 0.0f};
	std::vector<float> redVEC(red, red + 3);
	float white[3] = {255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f};
	std::vector<float> whiteVEC(white, white + 3);
	float light_blue[3] = {143.0f/255.0f,143.0f/255.0f,255.0f/255.0f};
	std::vector<float> light_blueVEC(light_blue, light_blue + 3);
	float yellow[3] = {255.0f/255.0f,200.0f/255.0f,50.0f/255.0f};
	std::vector<float> yellowVEC(yellow, yellow + 3);
	float orange[3] = {255.0f/255.0f,165.0f/255.0f,0.0f};
	std::vector<float> orangeVEC(orange, orange + 3);
	float green[3] = {0.0f,255.0f/255.0f,0.0f};
	std::vector<float> greenVEC(green, green + 3);
	float brown[3] = {165.0f/255.0f,42.0f/255.0f,42.0f/255.0f};
	std::vector<float> brownVEC(brown, brown + 3);
	float blue[3] = {0.0f,0.0f,255.0f/255.0f};
	std::vector<float> blueVEC(blue, blue + 3);
	float dark_green[3] = {42.0f/255.0f,128.0f/255.0f,42.0f/255.0f};
	std::vector<float> dark_greenVEC(dark_green, dark_green + 3);
	float dark_grey[3] = {128.0f/255.0f,128.0f/255.0f,128.0f/255.0f};
	std::vector<float> dark_greyVEC(dark_grey, dark_grey + 3);
	float deep_pink[3] = {255.0f/255.0f,20.0f/255.0f,147.0f/255.0f};
	std::vector<float> deep_pinkVEC(deep_pink, deep_pink + 3);


	colours["C"] = light_greyVEC;//Carbon
	colours["O"] = redVEC;//Oxygen
	colours["H"] = whiteVEC;//Hydrogen
	colours["N"] = light_blueVEC;//Nitrogen
	colours["S"] = yellowVEC;//Sulfur
	colours["P"] = orangeVEC;//Phosphorus
	colours["CL"] = greenVEC;//Chlorine
	colours["BR"] = brownVEC;//Bromine
	colours["ZN"] = brownVEC;//Zinc
	colours["NA"] = blueVEC;//Sodium
	colours["FE"] = orangeVEC;//Iron
	colours["F"] = orangeVEC;//Iron
	colours["MG"] = dark_greenVEC;//Magnesium
	colours["CA"] = dark_greyVEC;//Calcium
	colours["UNKNOWN"] = deep_pinkVEC;//Unknown molecule



}
