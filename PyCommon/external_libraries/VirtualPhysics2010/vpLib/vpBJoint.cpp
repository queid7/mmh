/*
VirtualPhysics v0.9

VirtualPhysics was developed as part of the project entitled "Development of 
Real-time Physics Simulation Engine for e-Entertainments" which was financially
supported by the grant from the strategic technology development program
(Project No. 2008-F-033-02) of both the MKE(Ministry of Knowledge Economy) and
MCST(Ministry of Culture, Sports and Tourism) of Korea.

Copyright (c) 2008-2010, Jinwook Kim, Korea Institute of Science and Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
	  and/or other materials provided with the distribution.

   3. Only research partners of the project "Development of Real-time Physics 
      Simulation Engine for e-Entertainments" can modify this list of 
	  conditions and the following disclaimer with the consent of the author, 
	  where the research partners refer to all principal investigators 
	  involved in the project. 

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO 
EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/


#include <VP/vpBJoint.h>
#include <VP/vpSystem.h>

vpBJoint::vpBJoint()
{
	for ( int i = 0; i < 3; i++ )
	{
		m_rQ[i] = m_rDq[i] = m_rDdq[i] = m_rActuationTau[i] = m_rSpringDamperTau[i] = m_rImpulsiveTau[i] = SCALAR_0;
		m_sS[i] = SCALAR_0;
		m_bHasUpperLimit[i] = m_bHasLowerLimit[i] = false;
	} 
	m_sVl = m_sDSdq = SCALAR_0;
	m_sSpringCoef = SpatialSpring(SCALAR_0);
	m_sDampingCoef = SpatialDamper(SCALAR_0);
}

void vpBJoint::BuildKinematics(void)
{
	GetState().push_back(vpState(this, 0));
	GetState().push_back(vpState(this, 1));
	GetState().push_back(vpState(this, 2));
}

void vpBJoint::SwapBody(void)
{
	vpJoint::SwapBody();
	
	m_sTi = Inv(m_sTi);
	m_rQ = LogR(Inv(Exp(m_rQ)));
}

#ifdef VP_PROTECT_SRC
	#include <VP/vpBJoint.inl>
#endif
