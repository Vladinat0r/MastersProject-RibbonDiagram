#include "CartoonModel.h"

void CartoonModel::calcCubicSpline(vector<Vector3d> points, vector<glm::vec3> norms, vector<glm::vec3> &splineSeg, vector<glm::vec3> &splineSegCAO) {
	// Load the kernel source code into the array source_str
	FILE *fp;
	char *source_str;
	size_t source_size;

	fp = fopen("calc_Cubic_Spline.cl", "r");
	if (!fp) {
		fprintf(stderr, "Failed to load kernel.\n");
		exit(1);
	}
	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

	// Get platform and device information
	cl_platform_id platform_id = NULL;
	cl_device_id device_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

	// Create an OpenCL context
	cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

	// Create a command queue
	cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);


	// Create memory buffers on the device for each vector
	cl_mem points_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(Vector3d) * points.size(), NULL, &ret);
	cl_mem norms_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(glm::vec3) * norms.size(), NULL, &ret);
	cl_mem splineSeg_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(glm::vec3) * splineSeg.size(), NULL, &ret);
	cl_mem splineSegCAO_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(glm::vec3) * splineSegCAO.size(), NULL, &ret);


	// Copy the lists A and B to their respective memory buffers
	ret = clEnqueueWriteBuffer(command_queue, points_mem_obj, CL_TRUE, 0, sizeof(Vector3d) * points.size(), points.data(), 0, NULL, NULL);
	ret = clEnqueueWriteBuffer(command_queue, norms_mem_obj, CL_TRUE, 0, sizeof(glm::vec3) * norms.size(), norms.data(), 0, NULL, NULL);


	// Create a program from the kernel source
	cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

	// Build the program
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	// Create the OpenCL kernel
	cl_kernel kernel = clCreateKernel(program, "calc_Cubic_Spline", &ret);


	// Set the arguments of the kernel
	ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&points_mem_obj);
	ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&norms_mem_obj);
	ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&splineSeg_mem_obj);
	ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&splineSegCAO_mem_obj);

	// Execute the OpenCL kernel on the list
	size_t global_item_size = LOD; // Process the entire lists
	size_t local_item_size = 2; //64 // Divide work items into groups of 64
	ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	
	// Read the memory buffer C on the device to the local variable C
	ret = clEnqueueReadBuffer(command_queue, splineSeg_mem_obj, CL_TRUE, 0, sizeof(glm::vec3) * splineSeg.size(), splineSeg.data(), 0, NULL, NULL);
	ret = clEnqueueReadBuffer(command_queue, splineSegCAO_mem_obj, CL_TRUE, 0, sizeof(glm::vec3) * splineSegCAO.size(), splineSegCAO.data(), 0, NULL, NULL);

	// Clean up
	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernel);
	ret = clReleaseProgram(program);

	ret = clReleaseMemObject(points_mem_obj);
	ret = clReleaseMemObject(norms_mem_obj);
	ret = clReleaseMemObject(splineSeg_mem_obj);
	ret = clReleaseMemObject(splineSegCAO_mem_obj);

	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
}

void CartoonModel::initCartoonModel(AtomList theList) {
	ClearCartoonModelData();

	vector<int> indexCA;

	vector<glm::vec3> residueOCNormals;

	unsigned int theListSize = theList.size();
	string lastResidue = theList[theListSize - 1]->getResidue();

	//Counters
	unsigned int residueCounter = stoi(theList[0]->getResidue());;
	unsigned int startResidueCounter = residueCounter;
	unsigned int numOfPoints = 0;

	//Shape size multiplier parameters
	float ribbonWidth = 12.0f;
	float ribbonHeight = 2.0f;

	bool isArrowhead = false;	//Flags last sub-segment as an arrowhead
	float arrowWidth = ribbonWidth * 1.5f;
	float arrowTipWidth = 1.0f;

	float tubeSize = ribbonHeight / 2.0f;

	//Parameters for tracking info about last/next segments
	string lastElemID = "N/A";
	string currentElemID;
	string currElemChainID;
	bool isNextElemTheSame = false;
	bool isLastElem = false;

	glm::mat4 rtMatrix, seg1Mat, seg2Mat;
	glm::vec3 lastBL2, lastBR2, lastTL2, lastTR2;

	glm::vec3 lastFrontNorm, lastBackNorm, lastLeftNorm, lastRightNorm, lastTopNorm, lastBottomNorm;

	vector<glm::vec3> alphaColours(numColours, glm::vec3(1.0f, 0.0f, 0.0f));
	vector<glm::vec3> betaColours(numColours, glm::vec3(0.0f, 0.0f, 1.0f));
	vector<glm::vec3> otherColours(numColours, glm::vec3(0.5f, 0.5f, 0.5f));

	unsigned int rainbowCounter = 0;
	bool isChangeRainbowColour = false;

	//1st ([i]) - Nitrogen atom
	//2nd ([i+1]) - Alpha Carbon atom
	//3rd ([i+2]) - Carbon atom
	//4th ([i+3]) - Oxygen atom
	//Finds and adds CA atoms to the points vector
	for (unsigned int i = 0; i < theListSize; i++) {
		//If chain break/end is reached, start building a new chain
		//stoi(theList[i]->getResidue()) < residueCounter
		if (isLastElem) {
			isChangeRainbowColour = true;
			isLastElem = false;
			numOfPoints = 0;
			indexCA.clear();
			residueOCNormals.clear();
			residueCounter--;
			
			//Locates the first residue number of the next chain
			for (int j = i; j < theListSize; j++) {
				string residueCompare = theList[j]->getResidue();
				if (stoi(residueCompare) != residueCounter) {
					i = j;
					residueCounter = stoi(residueCompare);
					startResidueCounter = residueCounter;
					break;
				}
			}
		}
		//Else continue the chain
		else if (theList[i]->getResidue()._Equal(std::to_string(residueCounter))) {
			indexCA.push_back(i + 1); //Push back the CA atom

			//Residue atoms
			//Vector3d N = theList[i]->getMyLocalPosition();		//Nitrogen atom coordinate
			Vector3d CA = theList[i + 1]->getMyLocalPosition();		//Alpha Carbon atom coordinate
			//Vector3d C = theList[i + 2]->getMyLocalPosition();		//Carbon atom coordinate
			Vector3d O = theList[i + 3]->getMyLocalPosition();		//Oxygen atom coordinate

			//CA to O direction normal
			glm::vec3 CAONormal = glm::normalize(glm::vec3(O.x, O.y, O.z) - glm::vec3(CA.x, CA.y, CA.z));

			//Creates binormal O atom directions
			if (residueCounter > startResidueCounter && glm::dot(CAONormal, residueOCNormals[numOfPoints-1]) < 0.0f) {
				residueOCNormals.push_back(-CAONormal); //Flipped structure plane normal
			}
			else {
				residueOCNormals.push_back(CAONormal); //Structure plane normal
			}

			residueCounter++;
			numOfPoints++;
		}


		if (numOfPoints >= 4) {
			vector<glm::vec3> splineSegment(LOD);
			vector<glm::vec3> splineSegmentCAO(LOD);
			Vector3d point0, point1, point2, point3;

			point0 = theList[indexCA[0]]->getMyLocalPosition();
			point1 = theList[indexCA[1]]->getMyLocalPosition();
			point2 = theList[indexCA[2]]->getMyLocalPosition();
			point3 = theList[indexCA[3]]->getMyLocalPosition();

			//OpenCL - alternative parameter and B-Spline function call
			/*vector<Vector3d> points = { theList[IndexCA[0]]->getMyLocalPosition(),
										theList[IndexCA[1]]->getMyLocalPosition(),
										theList[IndexCA[2]]->getMyLocalPosition(),
										theList[IndexCA[3]]->getMyLocalPosition()};
			calcCubicSpline(points, residueOCNormals, splineSegment, splineSegmentCAO);*/

			//Calculates Cubic B-Spline
			//Points 0 and 3 are knots, points 1 and 2 are segment start and end.
			for (unsigned int LODCount = 0; LODCount < LOD; ++LODCount) {
				float t = (float)LODCount / (LOD - 1);
				float it = 1.0f - t; // t value inverted

				//Calculate blending functions
				float t2 = t*t;
				float t3 = t*t*t;
				float it3 = it*it*it;

				float b0 = it3 / 6.0f;
				float b1 = (3 * t3 - 6 * t2 + 4) / 6.0f;
				float b2 = (-3 * t3 + 3 * t2 + 3 * t + 1) / 6.0f;
				float b3 = t3 / 6.0f;

				//Sum the control points mulitplied by their respective blending functions
				float x =	b0 * point0.x +
							b1 * point1.x +
							b2 * point2.x +
							b3 * point3.x;

				float y =	b0 * point0.y +
							b1 * point1.y +
							b2 * point2.y +
							b3 * point3.y;

				float z =	b0 * point0.z +
							b1 * point1.z +
							b2 * point2.z +
							b3 * point3.z;


				float xNorm =	b0 * (point0.x + residueOCNormals[0].x) +
								b1 * (point1.x + residueOCNormals[1].x) +
								b2 * (point2.x + residueOCNormals[2].x) +
								b3 * (point3.x + residueOCNormals[3].x);

				float yNorm =	b0 * (point0.y + residueOCNormals[0].y) +
								b1 * (point1.y + residueOCNormals[1].y) +
								b2 * (point2.y + residueOCNormals[2].y) +
								b3 * (point3.y + residueOCNormals[3].y);

				float zNorm =	b0 * (point0.z + residueOCNormals[0].z) +
								b1 * (point1.z + residueOCNormals[1].z) +
								b2 * (point2.z + residueOCNormals[2].z) +
								b3 * (point3.z + residueOCNormals[3].z);

				splineSegment[LODCount] = glm::vec3(x, y, z);
				splineSegmentCAO[LODCount] = glm::vec3(xNorm, yNorm, zNorm);
			}


			//Shape vertices and parameters
			int numVerts = (LOD - 1) * 36;
			vector<glm::vec3> shapeVerts(numVerts);
			vector<glm::vec3> shapeNormals(numVerts);
			unsigned int vertIndex = 0;	//Counter to move indices to the next segment shape.
			unsigned int normIndex = 0;	//Counter to move normals to the next segment shape.
			vector<glm::vec3> tempNormVector;

			glm::vec3 frontBottomLeft, frontBottomRight, frontTopRight, frontTopLeft, backBottomRight, backBottomLeft, backTopLeft, backTopRight;	//Shape vertices

			currentElemID = theList[indexCA[1]]->getSecondaryElementID();	//Current secondary structure type
			isNextElemTheSame = isNextSecondaryStructureID(theList, currentElemID, indexCA[2]);	//Checks if next element is the same as the current one.
			isLastElem = theList[indexCA[3]]->getIsLastSecondaryElementID(); //Check if this is the last element of the chain/end of protein

			//Creates rectangles for each spline segment
			int LODSize = LOD - 1;
			for (unsigned int p = 0; p < LODSize; p++) {
				glm::vec3 seg1CA = splineSegment[p];
				glm::vec3 seg2CA = splineSegment[p + 1];

				glm::vec3 seg1O = splineSegmentCAO[p];	
				glm::vec3 seg2O = splineSegmentCAO[p + 1];

				float activeWidth1, activeHeight1, activeWidth2, activeHeight2;
				
				//Sets width and height for current segment's structure type
				//If beta sheet
				if (currentElemID._Equal("E")) {
					if (p == (LOD - 2) && (!isNextElemTheSame || isLastElem)) { //If last segment and next residue is not a beta sheet, create an arrowhead at the end of a structure
						isArrowhead = true;
						activeWidth1 = arrowWidth;
						activeWidth2 = arrowTipWidth;
						activeHeight1 = ribbonHeight;
						activeHeight2 = ribbonHeight;
					}
					else {
						activeWidth1 = ribbonWidth;
						activeWidth2 = ribbonWidth;
						activeHeight1 = ribbonHeight;
						activeHeight2 = ribbonHeight;
					}
				}
				//Else if an alpha helix
				else if (currentElemID._Equal("H")) {
					activeWidth1 = ribbonWidth;
					activeWidth2 = ribbonWidth;
					activeHeight1 = ribbonHeight;
					activeHeight2 = ribbonHeight;
				}
				//Else a random coil
				else {
					activeWidth1 = tubeSize;
					activeWidth2 = tubeSize;
					activeHeight1 = ribbonHeight;
					activeHeight2 = ribbonHeight;
				}


				//seg2 (Back) parameters
				glm::vec3 seg2HalfWidth = glm::normalize(seg2O - seg2CA) * activeWidth2;
				backTopLeft = seg2CA - seg2HalfWidth;
				backTopRight = seg2O + seg2HalfWidth;
				//Matrix rotation and translation
				if (isArrowhead || //If this is an arrow head OR
					(p == 0 && //If first sub-segment AND...
					(lastElemID != "E" && currentElemID == "E") || //If last segment is not a beta-sheet and current segment is a beta-sheet, OR
					(lastElemID != "H" && currentElemID == "H") || //If last segment is not an alpha-helix and current segment is an alpha helix, OR
					((lastElemID == "H" || lastElemID == "E" || lastElemID == "N/A") && currentElemID != "H" && currentElemID != "E"))) { //If last segment was neither a beta-sheet or an alpha-helix and current segment is a random coil

					//seg1 (Front) parameters
					glm::vec3 seg1HalfWidth = glm::normalize(seg1O - seg1CA) * activeWidth1;
					frontTopLeft = seg1CA - seg1HalfWidth;
					frontTopRight = seg1O + seg1HalfWidth;
					frontBottomLeft = frontTopLeft + glm::normalize(glm::cross(backTopLeft - frontTopLeft, frontTopRight - frontTopLeft)) * activeHeight1;
					frontBottomRight = frontTopRight + glm::normalize(glm::cross(frontTopLeft - frontTopRight, backTopRight - frontTopRight)) * activeHeight1;
				}
				else  { //If not first iteration, connects segments together
					frontBottomLeft = lastBL2;
					frontBottomRight = lastBR2;
					frontTopLeft = lastTL2;
					frontTopRight = lastTR2;
				}
				//seg2 (Back) parameters
				backBottomLeft = backTopLeft + glm::normalize(glm::cross(backTopRight - backTopLeft, frontTopLeft - backTopLeft)) * activeHeight2;
				backBottomRight = backTopRight + glm::normalize(glm::cross(frontTopRight - backTopRight, backTopLeft - backTopRight)) * activeHeight2;

				//Connects segments together
				lastBL2 = backBottomLeft;
				lastBR2 = backBottomRight;

				lastTL2 = backTopLeft;
				lastTR2 = backTopRight;

				bool isFrontFace = true;
				bool isBackFace = true;
				bool isFrontAndBackFlipped = false;
				bool isLeftAndRightFlipped = false;
				bool isTopAndBottomFlipped = false;

				//Vertices - set anti-clockwise (from outside the shape perspective)
				//Front and Back faces
				glm::vec3 frontNorm = glm::normalize(glm::cross((frontBottomLeft - frontBottomRight), (frontTopRight - frontBottomRight)));
				glm::vec3 backNorm = glm::normalize(glm::cross((backBottomRight - backBottomLeft), (backTopLeft - backBottomLeft)));

				float frontAndBackFaceComparison = glm::dot(frontNorm, backNorm);

				if (frontAndBackFaceComparison > 1.0f) {
					isFrontAndBackFlipped = true;

					//Flip Front face
					frontNorm = glm::normalize(glm::cross((frontTopRight - frontBottomRight), (frontBottomLeft - frontBottomRight)));
					if ((p == 0 && currentElemID != lastElemID) || isArrowhead) {
						shapeVerts[vertIndex] =		frontTopRight;		//2
						shapeVerts[vertIndex + 1] = frontBottomRight;	//1
						shapeVerts[vertIndex + 2] = frontBottomLeft;	//0

						shapeVerts[vertIndex + 3] = frontBottomLeft;	//0
						shapeVerts[vertIndex + 4] = frontTopLeft;		//3
						shapeVerts[vertIndex + 5] = frontTopRight;		//2
					}
					else {
						isFrontFace = false;
						vertIndex -= 6;
					}

					//Flip Back face
					backNorm = glm::normalize(glm::cross((backTopLeft - backBottomLeft), (backBottomRight - backBottomLeft)));
					if (p == LOD - 2 && (!isNextElemTheSame || isLastElem)) {
						shapeVerts[vertIndex + 6] = backTopLeft;		//6
						shapeVerts[vertIndex + 7] = backBottomLeft;		//5
						shapeVerts[vertIndex + 8] = backBottomRight;	//4

						shapeVerts[vertIndex + 9] =	backBottomRight;	//4
						shapeVerts[vertIndex + 10] = backTopRight;		//7
						shapeVerts[vertIndex + 11] = backTopLeft;		//6
					}
					else {
						isBackFace = false;
						vertIndex -= 6;
					}
				}
				else {
					//Keep Front face
					if ((p == 0 && currentElemID != lastElemID) || isArrowhead) {
						shapeVerts[vertIndex] =		frontBottomLeft;	//0
						shapeVerts[vertIndex + 1] = frontBottomRight;	//1
						shapeVerts[vertIndex + 2] = frontTopRight;		//2

						shapeVerts[vertIndex + 3] = frontTopRight;		//2
						shapeVerts[vertIndex + 4] = frontTopLeft;		//3
						shapeVerts[vertIndex + 5] = frontBottomLeft;	//0
					}
					else {
						isFrontFace = false;
						vertIndex -= 6;
					}

					//Keep Back face
					if (p == LOD - 2 && (!isNextElemTheSame || isLastElem)) {
						shapeVerts[vertIndex + 6] = backBottomRight;	//4
						shapeVerts[vertIndex + 7] = backBottomLeft;		//5
						shapeVerts[vertIndex + 8] = backTopLeft;		//6

						shapeVerts[vertIndex + 9] = backTopLeft;		//6
						shapeVerts[vertIndex + 10] = backTopRight;		//7
						shapeVerts[vertIndex + 11] = backBottomRight;	//4
					}
					else {
						isBackFace = false;
						vertIndex -= 6;
					}
				}


				//Left and Right faces
				glm::vec3 leftNorm = glm::normalize(glm::cross((backBottomLeft - frontBottomLeft), (frontTopLeft - frontBottomLeft)));
				glm::vec3 rightNorm = glm::normalize(glm::cross((frontBottomRight - backBottomRight), (backTopRight - backBottomRight)));

				float leftAndRightFaceComparison = glm::dot(leftNorm, rightNorm);

				//Flip Left face
				if (leftAndRightFaceComparison > 1.0f) {
					isLeftAndRightFlipped = true;

					shapeVerts[vertIndex + 12] = frontTopLeft;		//3
					shapeVerts[vertIndex + 13] = frontBottomLeft;	//0
					shapeVerts[vertIndex + 14] = backBottomLeft;	//5

					shapeVerts[vertIndex + 15] = backBottomLeft;	//5
					shapeVerts[vertIndex + 16] = backTopLeft;		//6
					shapeVerts[vertIndex + 17] = frontTopLeft;		//3

					leftNorm = glm::normalize(glm::cross((frontTopLeft - frontBottomLeft), (backBottomLeft - frontBottomLeft)));


					//Flip Right face
					shapeVerts[vertIndex + 18] = backTopRight;		//7
					shapeVerts[vertIndex + 19] = backBottomRight;	//4
					shapeVerts[vertIndex + 20] = frontBottomRight;	//1

					shapeVerts[vertIndex + 21] = frontBottomRight;	//1
					shapeVerts[vertIndex + 22] = frontTopRight;		//2
					shapeVerts[vertIndex + 23] = backTopRight;		//7

					rightNorm = glm::normalize(glm::cross((backTopRight - backBottomRight), (frontBottomRight - backBottomRight)));
				}
				else {
					//Keep Left face
					shapeVerts[vertIndex + 12] = backBottomLeft;	//5
					shapeVerts[vertIndex + 13] = frontBottomLeft;	//0
					shapeVerts[vertIndex + 14] = frontTopLeft;		//3

					shapeVerts[vertIndex + 15] = frontTopLeft;		//3
					shapeVerts[vertIndex + 16] = backTopLeft;		//6
					shapeVerts[vertIndex + 17] = backBottomLeft;	//5

					//Keep Right face
					shapeVerts[vertIndex + 18] = frontBottomRight;	//1
					shapeVerts[vertIndex + 19] = backBottomRight;	//4
					shapeVerts[vertIndex + 20] = backTopRight;		//7

					shapeVerts[vertIndex + 21] = backTopRight;		//7
					shapeVerts[vertIndex + 22] = frontTopRight;		//2
					shapeVerts[vertIndex + 23] = frontBottomRight;	//1
				}


				//Top and Bottom faces
				glm::vec3 topNorm = glm::normalize(glm::cross((backTopRight - frontTopRight), (frontTopLeft - frontTopRight)));
				glm::vec3 bottomNorm = glm::normalize(glm::cross((frontBottomRight - backBottomRight), (backBottomLeft - backBottomRight)));

				float topAndBottomFaceComparison = glm::dot(glm::normalize(topNorm), glm::normalize(bottomNorm));

				if (topAndBottomFaceComparison > 1.0f) {
					isTopAndBottomFlipped = true;

					//Flip Top face
					shapeVerts[vertIndex + 24] = backTopRight;		//7
					shapeVerts[vertIndex + 25] = frontTopRight;		//2 
					shapeVerts[vertIndex + 26] = frontTopLeft;		//3

					shapeVerts[vertIndex + 27] = frontTopLeft;		//3
					shapeVerts[vertIndex + 28] = backTopLeft;		//6
					shapeVerts[vertIndex + 29] = backTopRight;		//7

					topNorm = glm::normalize(glm::cross((frontTopLeft - frontTopRight), (backTopRight - frontTopRight)));

					//Flip Bottom face
					shapeVerts[vertIndex + 30] = frontBottomRight;	//1
					shapeVerts[vertIndex + 31] = backBottomRight;	//4
					shapeVerts[vertIndex + 32] = backBottomLeft;	//5

					shapeVerts[vertIndex + 33] = backBottomLeft;	//5
					shapeVerts[vertIndex + 34] = frontBottomLeft;	//0
					shapeVerts[vertIndex + 35] = frontBottomRight;	//1

					bottomNorm = glm::normalize(glm::cross((backBottomLeft - backBottomRight), (frontBottomRight - backBottomRight)));
				}
				else {
					//Keep Top face
					shapeVerts[vertIndex + 24] = frontTopLeft;		//3
					shapeVerts[vertIndex + 25] = frontTopRight;		//2 
					shapeVerts[vertIndex + 26] = backTopRight;		//7

					shapeVerts[vertIndex + 27] = backTopRight;		//7
					shapeVerts[vertIndex + 28] = backTopLeft;		//6
					shapeVerts[vertIndex + 29] = frontTopLeft;		//3

					//Keep Bottom face
					shapeVerts[vertIndex + 30] = backBottomLeft;	//5
					shapeVerts[vertIndex + 31] = backBottomRight;	//4
					shapeVerts[vertIndex + 32] = frontBottomRight;	//1

					shapeVerts[vertIndex + 33] = frontBottomRight;	//1
					shapeVerts[vertIndex + 34] = frontBottomLeft;	//0
					shapeVerts[vertIndex + 35] = backBottomLeft;	//5
				}


				if (isFrontFace) {
					if (isFrontAndBackFlipped) {
						//Flip Front Normals
						shapeNormals[normIndex] = frontNorm;			//2
						shapeNormals[normIndex + 1] = frontNorm;	//1
						shapeNormals[normIndex + 2] = frontNorm;	//0

						shapeNormals[normIndex + 3] = frontNorm;	//0
						shapeNormals[normIndex + 4] = frontNorm;		//3
						shapeNormals[normIndex + 5] = frontNorm;		//2
					}
					else {
						//Keep Front Normals
						shapeNormals[normIndex] = frontNorm;		//0
						shapeNormals[normIndex + 1] = frontNorm;	//1
						shapeNormals[normIndex + 2] = frontNorm;		//2

						shapeNormals[normIndex + 3] = frontNorm;		//2
						shapeNormals[normIndex + 4] = frontNorm;		//3
						shapeNormals[normIndex + 5] = frontNorm;	//0
					}
				}
				else {
					normIndex -= 6;
				}

				if (isBackFace) {
					if (isFrontAndBackFlipped) {
						//Flip Back Normals
						shapeNormals[normIndex + 6] = backNorm;		//6
						shapeNormals[normIndex + 7] = backNorm;	//5
						shapeNormals[normIndex + 8] = backNorm;	//4

						shapeNormals[normIndex + 9] = backNorm;	//4
						shapeNormals[normIndex + 10] = backNorm;		//7
						shapeNormals[normIndex + 11] = backNorm;		//6
					}
					else {
						//Keep Back Normals
						shapeNormals[normIndex + 6] = backNorm;	//4
						shapeNormals[normIndex + 7] = backNorm;	//5
						shapeNormals[normIndex + 8] = backNorm;		//6

						shapeNormals[normIndex + 9] = backNorm;		//6
						shapeNormals[normIndex + 10] = backNorm;		//7
						shapeNormals[normIndex + 11] = backNorm;	//4
					}
				}
				else {
					normIndex -= 6;
				}

				if (isLeftAndRightFlipped) {
					//Flip Left Normals
					shapeNormals[normIndex + 12] = lastLeftNorm;		//3
					shapeNormals[normIndex + 13] = leftNorm;	//0
					shapeNormals[normIndex + 14] = leftNorm;	//5

					shapeNormals[normIndex + 15] = leftNorm;	//5
					shapeNormals[normIndex + 16] = leftNorm;		//6
					shapeNormals[normIndex + 17] = lastLeftNorm;		//3

					//Flip Right Normals
					shapeNormals[normIndex + 18] = rightNorm;		//7
					shapeNormals[normIndex + 19] = rightNorm;	//4
					shapeNormals[normIndex + 20] = lastRightNorm;	//1

					shapeNormals[normIndex + 21] = lastRightNorm;	//1
					shapeNormals[normIndex + 22] = lastRightNorm;	//2
					shapeNormals[normIndex + 23] = rightNorm;		//7
				}
				else {
					//Keep Left Normals
					shapeNormals[normIndex + 12] = leftNorm;	//5
					shapeNormals[normIndex + 13] = lastLeftNorm;	//0
					shapeNormals[normIndex + 14] = lastLeftNorm;		//3

					shapeNormals[normIndex + 15] = lastLeftNorm;		//3
					shapeNormals[normIndex + 16] = leftNorm;		//6
					shapeNormals[normIndex + 17] = leftNorm;	//5

					//Keep Right Normals
					shapeNormals[normIndex + 18] = lastRightNorm;	//1
					shapeNormals[normIndex + 19] = rightNorm;	//4
					shapeNormals[normIndex + 20] = rightNorm;		//7

					shapeNormals[normIndex + 21] = rightNorm;		//7
					shapeNormals[normIndex + 22] = lastRightNorm;	//2
					shapeNormals[normIndex + 23] = lastRightNorm;	//1
				}

				if (isTopAndBottomFlipped) {
					//Flip Top Normals
					shapeNormals[normIndex + 24] = topNorm;		//7
					shapeNormals[normIndex + 25] = lastTopNorm;	//2 
					shapeNormals[normIndex + 26] = lastTopNorm;		//3

					shapeNormals[normIndex + 27] = lastTopNorm;		//3
					shapeNormals[normIndex + 28] = topNorm;		//6
					shapeNormals[normIndex + 29] = topNorm;		//7

					//Flip Bottom Normals
					shapeNormals[normIndex + 30] = lastBottomNorm;	//1
					shapeNormals[normIndex + 31] = bottomNorm;	//4
					shapeNormals[normIndex + 32] = bottomNorm;	//5

					shapeNormals[normIndex + 33] = bottomNorm;	//5
					shapeNormals[normIndex + 34] = lastBottomNorm;	//0
					shapeNormals[normIndex + 35] = lastBottomNorm;	//1
				}
				else {
					//Keep Top Normals
					shapeNormals[normIndex + 24] = lastTopNorm;		//3
					shapeNormals[normIndex + 25] = lastTopNorm;	//2 
					shapeNormals[normIndex + 26] = topNorm;		//7

					shapeNormals[normIndex + 27] = topNorm;		//7
					shapeNormals[normIndex + 28] = topNorm;		//6
					shapeNormals[normIndex + 29] = lastTopNorm;		//3

					//Keep Bottom Normals
					shapeNormals[normIndex + 30] = bottomNorm;	//5
					shapeNormals[normIndex + 31] = bottomNorm;	//4
					shapeNormals[normIndex + 32] = lastBottomNorm;	//1

					shapeNormals[normIndex + 33] = lastBottomNorm;	//1
					shapeNormals[normIndex + 34] = lastBottomNorm;	//0
					shapeNormals[normIndex + 35] = bottomNorm;	//5
				}

				lastFrontNorm = frontNorm;
				lastBackNorm = backNorm;
				lastLeftNorm = leftNorm;
				lastRightNorm = rightNorm;
				lastTopNorm = topNorm;
				lastBottomNorm = bottomNorm;

				normIndex += 36;
				vertIndex += 36;	//Increment for next cube

				isArrowhead = false;
				lastElemID = currentElemID;
			}


			////If alpha helix
			//if (currentElemID._Equal("H")) {
			//	addCartoonParams(shapeVerts, shapeNormals, alphaColours);
			//}
			////Else if beta sheet
			//else if (currentElemID._Equal("E")) {
			//	addCartoonParams(shapeVerts, shapeNormals, betaColours);
			//}
			////Else other unclassified structures
			//else {
			//	addCartoonParams(shapeVerts, shapeNormals, otherColours);
			//}


			if (isChangeRainbowColour) {
				if (rainbowCounter == 5) {
					rainbowCounter = 0;
				}
				else {
					rainbowCounter++;
				}
				isChangeRainbowColour = false;
			}
			addCartoonParams(shapeVerts, shapeNormals, rainbowColours[rainbowCounter]);


			numOfPoints--;
			indexCA.erase(indexCA.begin());
			residueOCNormals.erase(residueOCNormals.begin());

			splineSegment.clear();
			splineSegmentCAO.clear();

			shapeVerts.clear();
			shapeNormals.clear();
		}
	}
	constructCartoonBuffers(cartoonShader.handle());
}


//Checks if the next residue is of the same secondary structure type. Returns true if types are the same.
bool CartoonModel::isNextSecondaryStructureID(AtomList theList, string type, int iPos) {
	if (theList[iPos]->getSecondaryElementID()._Equal(type)) {
		return true;
	}
	else {
		return false;
	}
}


void CartoonModel::drawCartoonModel(glm::mat4 ProjectionMatrix, glm::vec2 xyMove, float ZOOM, float SPIN, bool isTransparent) {
	SuperAtom::bindbuffers(cartoonShader.handle());

	applyCartoonShaderParams(ProjectionMatrix, xyMove, ZOOM, SPIN, isTransparent);

	renderCartoonUsingVBO(cartoonShader.handle());

	SuperAtom::Unbind(cartoonShader.handle());
}


void CartoonModel::applyCartoonShaderParams(glm::mat4 ProjectionMatrix, glm::vec2 xyMove, float ZOOM, float SPIN, bool isTransparent) {
	glUseProgram(cartoonShader.handle());

	//Setup shaders
	GLuint matLocation = glGetUniformLocation(cartoonShader.handle(), "ProjectionMatrix");
	glUniformMatrix4fv(matLocation, 1, GL_FALSE, &ProjectionMatrix[0][0]);
	glm::vec3 cameraPos = glm::vec3(-xyMove, -ZOOM);
	ModelViewMatrixCartoon = glm::translate(glm::mat4(1.0), cameraPos);
	ModelViewMatrixCartoon = glm::rotate(ModelViewMatrixCartoon, SPIN, glm::vec3(0.0f, 1.0f, 0.0f));
	glUniformMatrix4fv(glGetUniformLocation(cartoonShader.handle(), "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ModelViewMatrixCartoon));

	//normal matrix isn't effected by translation
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(ModelViewMatrixCartoon));
	glUniformMatrix3fv(glGetUniformLocation(cartoonShader.handle(), "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

	float Light_Ambient[4] = { 0.8f, 0.8f, 0.8f, 1.0f };
	glUniform4fv(glGetUniformLocation(cartoonShader.handle(), "light_ambient"), 1, Light_Ambient);
	float Light_Diffuse[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
	glUniform4fv(glGetUniformLocation(cartoonShader.handle(), "light_diffuse"), 1, Light_Diffuse);
	float Light_Specular[4] = { 0.9f, 0.9f, 0.9f, 1.0f };
	glUniform4fv(glGetUniformLocation(cartoonShader.handle(), "light_specular"), 1, Light_Specular);

	float Material_Ambient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glUniform4fv(glGetUniformLocation(cartoonShader.handle(), "material_ambient"), 1, Material_Ambient);
	float Material_Diffuse[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glUniform4fv(glGetUniformLocation(cartoonShader.handle(), "material_diffuse"), 1, Material_Diffuse);
	float Material_Specular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };
	glUniform4fv(glGetUniformLocation(cartoonShader.handle(), "material_specular"), 1, Material_Specular);
	float Material_Shininess = 100.0f;
	glUniform1f(glGetUniformLocation(cartoonShader.handle(), "material_shininess"), Material_Shininess);

	float LightPos[4] = { -15.0f, 2000.0f, 2500.0f, 1.0f };
	glUniform4fv(glGetUniformLocation(cartoonShader.handle(), "LightPos"), 1, LightPos);

	glUniform1i(glGetUniformLocation(cartoonShader.handle(), "isTransparent"), isTransparent);

	glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0), SPIN, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 rotatedCameraPos = glm::vec3(glm::vec4(cameraPos, 1.0f) * rotationMat);
	float cameraPosTransparency[3] = {rotatedCameraPos.x, rotatedCameraPos.y, rotatedCameraPos.z};
	glUniform3fv(glGetUniformLocation(cartoonShader.handle(), "cameraPos"), 1, cameraPosTransparency);
}


void CartoonModel::loadShader(const char* vertPath, const char* fragPath) {
	if (!cartoonShader.load("cartoonModelShader", vertPath, fragPath))
	{
		cout << "Failed to load cartoonModelShader" << endl;
	}
	glUseProgram(cartoonShader.handle());
}

void CartoonModel::ClearCartoonModelData() {
	cartoonVertices.clear();
	cartoonNormals.clear();
	cartoonColours.clear();
}

//Cartoon model rendering
void CartoonModel::addCartoonParams(vector<glm::vec3>& newShapeVerts, vector<glm::vec3>& newNormals, vector<glm::vec3>& newColours) { //, vector<int>& newIndeces) {
	cartoonVertices.insert(cartoonVertices.end(), newShapeVerts.begin(), newShapeVerts.end());
	cartoonNormals.insert(cartoonNormals.end(), newNormals.begin(), newNormals.end());
	cartoonColours.insert(cartoonColours.end(), newColours.begin(), newColours.end());
}

void CartoonModel::renderCartoonUsingVBO(int shader) {
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrixCartoon[0][0]);
	renderCartoonVBO(shader);
}

void CartoonModel::renderCartoonVBO(int shader) {
	glBindVertexArray(cartoonVAOID);
	glDrawArrays(GL_TRIANGLES, 0, cartoonVertices.size());
}

void CartoonModel::constructCartoonBuffers(int shader) {
	if (!isCartoonVBOBuilt) {
		// VAO allocation
		glGenVertexArrays(1, &cartoonVAOID);

		// First VAO setup
		glBindVertexArray(cartoonVAOID);

		glGenBuffers(3, cartoonVBOID);

		isCartoonVBOBuilt = true;
	}

	glBindBuffer(GL_ARRAY_BUFFER, cartoonVBOID[0]);
	glBufferData(GL_ARRAY_BUFFER, cartoonVertices.size() * sizeof(float) * 3, cartoonVertices.data(), GL_STATIC_DRAW);	//Vertices assignment
	GLint vertexLocation = glGetAttribLocation(shader, "in_Position");
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vertexLocation);

	glBindBuffer(GL_ARRAY_BUFFER, cartoonVBOID[1]);
	glBufferData(GL_ARRAY_BUFFER, cartoonColours.size() * sizeof(float) * 3, cartoonColours.data(), GL_STATIC_DRAW);		//Colour assignment
	GLint colorLocation = glGetAttribLocation(shader, "in_Color");
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(colorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, cartoonVBOID[2]);
	glBufferData(GL_ARRAY_BUFFER, cartoonNormals.size() * sizeof(float) * 3, cartoonNormals.data(), GL_STATIC_DRAW);		//Normal assignment
	GLint normalLocation = glGetAttribLocation(shader, "in_Normal");
	glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(normalLocation);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	////Buffer indexing
	//glGenBuffers(1, &cartoonIBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cartoonIBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, cartoonIndices.size() * sizeof(float), cartoonIndices.data(), GL_STATIC_DRAW);	//Indeces assignment
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}



	////Catmull-Rom spline algorithm
	////Spline parametters
	//float alpha = 0.5f;
	//float tension = 0.0f;	//Ranges from 0 to 1
	
	//if (numOfPoints >= 4) {
		//	vector<glm::vec3> splineSegment(LOD);
		//	vector<glm::vec3> splineSegmentCAO(LOD);
		//	glm::vec3 point0, point1, point2, point3;
		//	glm::vec3 point0Norm, point1Norm, point2Norm, point3Norm;
		//	Vector3d temp;

		//	//Point coordinates
		//	temp = theList[indexCA[0]]->getMyLocalPosition();
		//	point0 = glm::vec3(temp.x, temp.y, temp.z);
		//	temp = theList[indexCA[1]]->getMyLocalPosition();
		//	point1 = glm::vec3(temp.x, temp.y, temp.z);
		//	temp = theList[indexCA[2]]->getMyLocalPosition();
		//	point2 = glm::vec3(temp.x, temp.y, temp.z);
		//	temp = theList[indexCA[3]]->getMyLocalPosition();
		//	point3 = glm::vec3(temp.x, temp.y, temp.z);

		//	//Normals
		//	point0Norm = point0 + residueOCNormals[0];
		//	point1Norm = point1 + residueOCNormals[1];
		//	point2Norm = point2 + residueOCNormals[2];
		//	point3Norm = point3 + residueOCNormals[3];

		//	//Catmull rom spline formula
		//	for (unsigned int LODCount = 0; LODCount < LOD; ++LODCount) {
		//		float t = (float)LODCount / (LOD - 1);
		//		//float it = 1.0f - t; // t value inverted

		//		float t01 = pow(glm::distance(point0, point1), alpha);
		//		float t12 = pow(glm::distance(point1, point2), alpha);
		//		float t23 = pow(glm::distance(point2, point3), alpha);
		//		glm::vec3 m1 = (1.0f - tension) * (point2 - point1 + t12 * ((point1 - point0) / t01 - (point2 - point0) / (t01 + t12)));
		//		glm::vec3 m2 = (1.0f - tension) * (point2 - point1 + t12 * ((point3 - point2) / t23 - (point3 - point1) / (t12 + t23)));
		//		glm::vec3 a = 2.0f * (point1 - point2) + m1 + m2;
		//		glm::vec3 b = -3.0f * (point1 - point2) - m1 - m1 - m2;
		//		glm::vec3 c = m1;
		//		glm::vec3 d = point1;
		//		glm::vec3 newPoint =	a * t * t * t +
		//								b * t * t +
		//								c * t +
		//								d;

		//		t01 = pow(glm::distance(point0Norm, point1Norm), alpha);
		//		t12 = pow(glm::distance(point1Norm, point2Norm), alpha);
		//		t23 = pow(glm::distance(point2Norm, point3Norm), alpha);
		//		m1 = (1.0f - tension) * (point2Norm - point1Norm + t12 * ((point1Norm - point0Norm) / t01 - (point2Norm - point0Norm) / (t01 + t12)));
		//		m2 = (1.0f - tension) * (point2Norm - point1Norm + t12 * ((point3Norm - point2Norm) / t23 - (point3Norm - point1Norm) / (t12 + t23)));
		//		a = 2.0f * (point1Norm - point2Norm) + m1 + m2;
		//		b = -3.0f * (point1Norm - point2Norm) - m1 - m1 - m2;
		//		c = m1;
		//		d = point1Norm;
		//		glm::vec3 newPointNorm =	a * t * t * t +
		//									b * t * t +
		//									c * t +
		//									d;

		//		splineSegment[LODCount] = newPoint;
		//		splineSegmentCAO[LODCount] = newPointNorm;
		//	}
