/*!
 * \file variable_direct_elasticity.cpp
 * \brief Definition of the solution fields.
 * \author F. Palacios, R. Sanchez
 * \version 3.2.8.2 "eagle"
 *
 * SU2 Lead Developers: Dr. Francisco Palacios (fpalacios@stanford.edu).
 *                      Dr. Thomas D. Economon (economon@stanford.edu).
 *
 * SU2 Developers: Prof. Juan J. Alonso's group at Stanford University.
 *                 Prof. Piero Colonna's group at Delft University of Technology.
 *                 Prof. Nicolas R. Gauger's group at Kaiserslautern University of Technology.
 *                 Prof. Alberto Guardone's group at Polytechnic University of Milan.
 *                 Prof. Rafael Palacios' group at Imperial College London.
 *
 * Copyright (C) 2012-2015 SU2, the open-source CFD code.
 *
 * SU2 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * SU2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SU2. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../include/variable_structure.hpp"

CFEAVariable::CFEAVariable(void) : CVariable() { }

CFEAVariable::CFEAVariable(double *val_fea, unsigned short val_nDim, unsigned short val_nvar, CConfig *config)
: CVariable(val_nDim, val_nvar, config) {
	unsigned short iVar, iDim, jDim;

	bool dynamic = (config->GetDynamic_Analysis() == DYNAMIC);
  
	/*--- Allocate residual structures ---*/
	Residual_Sum = new double [nVar]; Residual_Old = new double [nVar];
  
	/*--- Initialization of variables ---*/
	for (iVar = 0; iVar < nVar; iVar++) {
		Solution[iVar] = val_fea[iVar];
		Solution_Old[iVar] = val_fea[iVar];
	}

	if (dynamic){

		/*--- Allocate solution structures ---*/
		Solution_Pred =  new double [nVar];
		Solution_time_n = new double [nVar];
		Solution_Vel = new double [nVar];
		Solution_Vel_time_n = new double [nVar];
		Solution_Accel = new double [nVar];
		Solution_Accel_time_n = new double [nVar];

		/*--- Initialization of variables for dynamic problem ---*/
		for (iVar = 0; iVar < nVar; iVar++) {
			Solution_Pred[iVar] =  val_fea[iVar];
			Solution_time_n[iVar] = val_fea[iVar];
			Solution_Vel[iVar] = val_fea[iVar];
			Solution_Vel_time_n[iVar] = val_fea[iVar];
			Solution_Accel[iVar] = val_fea[iVar];
			Solution_Accel_time_n[iVar] = val_fea[iVar];
		}
	}


  
  /*--- Allocate stress tensor ---*/
	Stress = new double* [nDim];
	for (iDim = 0; iDim < nDim; iDim++)
		Stress[iDim] = new double [nDim];

  /*--- Initialize stress tensor---*/
	for (iDim = 0; iDim < nDim; iDim++){
		for (jDim = 0; jDim < nDim; jDim++){
			Stress[iDim][jDim]=0.0;
		}
	}
}

CFEAVariable::~CFEAVariable(void) {
  unsigned short iDim;

	for (iDim = 0; iDim < nDim; iDim++)
		delete [] Stress[iDim];
	delete [] Stress;
  delete [] Residual_Sum;
  delete [] Residual_Old;
  
}

void CFEAVariable::SetSolution_time_n(void) {

	for (unsigned short iVar = 0; iVar < nVar; iVar++)
		Solution_time_n[iVar] = Solution[iVar];

}

void CFEAVariable::SetSolution_time_n(double *val_solution_time_n) {

	for (unsigned short iVar = 0; iVar < nVar; iVar++)
		Solution_time_n[iVar] = val_solution_time_n[iVar];

}

void CFEAVariable::SetSolution_Vel(double *val_solution_vel) {

	for (unsigned short iVar = 0; iVar < nVar; iVar++)
		Solution_Vel[iVar] = val_solution_vel[iVar];

}

void CFEAVariable::SetSolution_Vel_time_n(double *val_solution_vel_time_n) {

	for (unsigned short iVar = 0; iVar < nVar; iVar++)
		Solution_Vel_time_n[iVar] = val_solution_vel_time_n[iVar];

}

void CFEAVariable::SetSolution_Vel_time_n(void) {

	for (unsigned short iVar = 0; iVar < nVar; iVar++)
		Solution_Vel_time_n[iVar] = Solution_Vel[iVar];

}

void CFEAVariable::SetSolution_Accel(double *val_solution_accel) {

	for (unsigned short iVar = 0; iVar < nVar; iVar++)
		Solution_Accel[iVar] = val_solution_accel[iVar];

}

void CFEAVariable::SetSolution_Accel_time_n(double *val_solution_accel_time_n) {

	for (unsigned short iVar = 0; iVar < nVar; iVar++)
		Solution_Accel_time_n[iVar] = val_solution_accel_time_n[iVar];

}

void CFEAVariable::SetSolution_Accel_time_n(void) {

	for (unsigned short iVar = 0; iVar < nVar; iVar++)
		Solution_Accel_time_n[iVar] = Solution_Accel[iVar];

}

void CFEAVariable::SetSolution_Pred(void){

	for (unsigned short iVar = 0; iVar < nVar; iVar++)
		Solution_Pred[iVar] = Solution[iVar];

}



CFEABoundVariable::CFEABoundVariable(void) : CVariable() { }

CFEABoundVariable::CFEABoundVariable(unsigned short val_nDim, unsigned short val_nvar, unsigned short val_nElBound, CConfig *config)
: CVariable(val_nDim, val_nvar, config) {
	unsigned short iVar, iDim, jDim;

	/*--- Allocate residual structures ---*/
	Residual_Sum = new double [nVar]; Residual_Old = new double [nVar];

	/*--- Allocate stress tensor ---*/
	if (nDim == 2){
		Traction = new double* [2*nDim];
		for (iDim = 0; iDim < 2*nDim ; iDim++)
			Traction[iDim] = new double [val_nElBound];
	}
	else if (nDim == 3){
		/*--- Allocate stress tensor ---*/
		Traction = new double* [4*nDim];
		for (iDim = 0; iDim < 4*nDim ; iDim++)
			Traction[iDim] = new double [val_nElBound];
	}

	/*--- Initialize stress tensor ---*/
	if (nDim == 2){
		/*--- Initialize stress tensor---*/
		for (iDim = 0; iDim < 2*nDim; iDim++){
			for (jDim = 0; jDim < val_nElBound; jDim++){
				Traction[iDim][jDim]=0.0;
			}
		}
	}
	else if (nDim == 3){
		/*--- Initialize stress tensor---*/
		for (iDim = 0; iDim < 4*nDim; iDim++){
			for (jDim = 0; jDim < val_nElBound; jDim++){
				Traction[iDim][jDim]=0.0;
			}
		}
	}

}

CFEABoundVariable::~CFEABoundVariable(void) {
  unsigned short iDim;

	/*--- Initialize stress tensor ---*/
	if (nDim == 2){
		/*--- Initialize stress tensor---*/
		for (iDim = 0; iDim < 2*nDim; iDim++){
			delete [] Traction[iDim];
		}
	}
	else if (nDim == 3){
		/*--- Initialize stress tensor---*/
		for (iDim = 0; iDim < 4*nDim; iDim++){
			delete [] Traction[iDim];
		}
	}

	delete [] Traction;

  delete [] Residual_Sum;
  delete [] Residual_Old;
  
}
