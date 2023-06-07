#include <iostream>
#include "PDB.h"
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/norm.hpp>


// Force Objects -----------------------------------------------------------------------------------------------
void SpringForce(Particle* p1, Particle* p2, float ks = 5.0f, float kd = 0.2f, float l0 = 1.0f)
{
	glm::vec3 pos1{p1->p[0], p1->p[1], 0};
	glm::vec3 pos2{p2->p[0], p2->p[1], 0};
	glm::vec3 pos12 = pos2 - pos1;
	float pmag = glm::length(pos12);
	glm::vec3 vel1{p1->v[0], p1->v[1], 0};
	glm::vec3 vel2{p2->v[0], p2->v[1], 0};
	glm::vec3 vel12 = vel2 - vel1;

	glm::vec3 spring_force = (ks * abs((pmag - l0)) + kd * glm::dot(vel12, pos12)/pmag) * glm::normalize(pos12);

	p1->f[0] += spring_force[0];
	p1->f[1] += spring_force[1];
	p1->f[2] += spring_force[2];
	p2->f[0] -= spring_force[0];
	p2->f[1] -= spring_force[1];
	p2->f[2] -= spring_force[2];
}



// Particle System Functions ----------------------------------------------------------------------------------
ParticleSystem::ParticleSystem()
{
	n = int(Particles.size());
}

void ParticleSystem::AddParticle(Particle* p)
{
	Particles.push_back(*p);
	n = int(Particles.size());
}


void ParticleSystem::ClearForces()
{
	for(int i=0; i<n; ++i)
	{
		Particles[i].f = {0.0f,0.0f,0.0f};
	}
}

void ParticleSystem::CalculateForces()
{
	for (int i{0}; i<Particles.size(); ++i)
	{
		Particles[i].f[1] += Gravity - Drag * Particles[i].v[1];
	}
	
	for(int i=0; i<int(SpringParticles.size()); ++i)
	{
		SpringForce(&Particles[SpringParticles[i][0]],
			&Particles[SpringParticles[i][1]]);
	}
	
}


// ODE Solver --------------------------------------------------------------------------------------------------------------------------------------
int ParticleDim(const ParticleSystem* ps)
{
	return 6 * ps->n;
}

void GetParticleState(const ParticleSystem* ps, vector<float>* dst)
{
	for(Particle p: ps->Particles)
	{
		dst->push_back(p.p[0]);
		dst->push_back(p.p[1]);
		dst->push_back(p.p[2]);
		dst->push_back(p.v[0]);
		dst->push_back(p.v[1]);
		dst->push_back(p.p[2]);
	}
}

void SetParticleState(ParticleSystem* ps, vector<float>* src)
{
	for (int i=1; i < ps->n; ++i)
	{
		ps->Particles[i].p[0] = src->at(6 * i);
		ps->Particles[i].p[1] = src->at(6 * i + 1);
		ps->Particles[i].p[2] = src->at(6 * i + 2);
		ps->Particles[i].v[0] = src->at(6 * i + 3);
		ps->Particles[i].v[1] = src->at(6 * i + 4);
		ps->Particles[i].v[2] = src->at(6 * i + 5);
	}
}

void ParticleDerivative(ParticleSystem* ps, vector<float>* dst)
{
	ps->ClearForces();
	ps->CalculateForces();

	for (Particle p : ps->Particles)
	{
		dst->push_back(p.v[0]);
		dst->push_back(p.v[1]);
		dst->push_back(p.v[2]);
		dst->push_back(p.f[0] / p.m);
		dst->push_back(p.f[1] / p.m);
		dst->push_back(p.f[2] / p.m);
	}
}

void ScaleUp(vector<float>* v1, vector<float>* v2, float h)
{
		for (int i = 0; i<int(v1->size()); ++i)
		{
			v1->at(i) = v1->at(i) + v2->at(i) * h;
		}
}

void EulerSolver(ParticleSystem* ps, float DeltaT)
{
	vector<float> temp1{};
	vector<float> temp2{};
	GetParticleState(ps, &temp1);
	ParticleDerivative(ps, &temp2);
	ScaleUp(&temp1, &temp2, DeltaT);
	SetParticleState(ps, &temp1);
	ps->time += DeltaT;
}

// To Do:
//		Create Solver Class 
//		Use GLM for calculations
