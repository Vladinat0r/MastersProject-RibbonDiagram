/************************************************

Datatables: 
File to store information about proteins
Standard colouring and radius information 
for each element is stored.

***************************************************/

#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <string>

const float INITIAL_SCALE = 10.0;

class DataTables
{
public:
	typedef std::map <std::string, std::vector<float>> colour_list;
public:
	typedef std::map <std::string, float> VDW_Radii_list;
public:
	DataTables(void);
public:
	virtual ~DataTables(void);
	colour_list getColourList(){return colours;}
	VDW_Radii_list getVDWRadii(){return VDW_List;}

private:

	colour_list colours;
	VDW_Radii_list VDW_List;


	void create_CPK_ColourList();
	void create_VanDerWaals_AtomicRadii();


};
