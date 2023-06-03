#include <iostream>
#include "PDB.h"

// Global Variables --------------------------------------------------------------------------------------------
vector<float> temp1{};
vector<float> temp2{};
vector<array<int, 2>> SpringParticles{};
vector<array<float, 2>> SpringConsts{};
vector<array<float, 3>> SpringLengths{};


// Force Objects -----------------------------------------------------------------------------------------------
void SpringForce(Particle* p1, Particle* p2, float ks, float kd, array<float, 3> l0)
{
	float p120 = p1->p[0] - p2->p[0];
	float p121 = p1->p[1] - p2->p[1];
	float p122 = p1->p[2] - p2->p[2];
	float v120 = p1->v[0] - p2->v[0];
	float v121 = p1->v[1] - p2->v[1];
	float v122 = p1->v[2] - p2->v[2];

	float f10 = -(ks * (abs(p120) - l0[0]) + kd * abs(v120));
	float f11 = -(ks * (abs(p121) - l0[1]) + kd * abs(v121));
	float f12 = -(ks * (abs(p122) - l0[2]) + kd * abs(v122));

	p1->f[0] += f10;
	p1->f[1] += f11;
	p1->f[2] += f12;
	p2->f[0] += -f10;
	p2->f[1] += -f11;
	p2->f[2] += -f12;
}



// Particle System Functions ----------------------------------------------------------------------------------
ParticleSystem::ParticleSystem()
{
	n = int(Particles.size());
}

void ParticleSystem::AddParticle(Particle* p)
{
	Particles.push_back(p);
	n = int(Particles.size());
}

void ParticleSystem::ClearForces()
{
	for(Particle* p: Particles)
	{
		p->f = { 0.0f,0.0f,0.0f };
	}
}

void ParticleSystem::CalculateForces()
{
	for(Particle* p: Particles)
	{
		p->f[0] += Gravity - Drag * p->v[0];
		p->f[1] += Gravity - Drag * p->v[1];
		p->f[2] += Gravity - Drag * p->v[2];
	}

	for(int i=0; i<int(SpringParticles.size()); ++i)
	{
		SpringForce(Particles[SpringParticles[i][0]], 
			Particles[SpringParticles[i][1]], 
			SpringConsts[i][0], 
			SpringConsts[i][1], 
			SpringLengths[i]);
	}
}


// ODE Solver --------------------------------------------------------------------------------------------------------------------------------------
int ParticleDim(const ParticleSystem* ps)
{
	return 6 * ps->n;
}

void GetParticleState(const ParticleSystem* ps, vector<float>* dst)
{
	for(Particle* p: ps->Particles)
	{
		dst->push_back(p->p[0]);
		dst->push_back(p->p[1]);
		dst->push_back(p->p[2]);
		dst->push_back(p->v[0]);
		dst->push_back(p->v[1]);
		dst->push_back(p->p[2]);
	}
}

void SetParticleState(const ParticleSystem* ps, vector<float>* src)
{
	for (int i=0; i < ps->n; ++i)
	{
		ps->Particles[i]->p[0] = src->at(6 * i);
		ps->Particles[i]->p[1] = src->at(6 * i + 1);
		ps->Particles[i]->p[2] = src->at(6 * i + 2);
		ps->Particles[i]->v[0] = src->at(6 * i + 3);
		ps->Particles[i]->v[1] = src->at(6 * i + 4);
		ps->Particles[i]->v[2] = src->at(6 * i + 5);
	}
}

void ParticleDerivative(ParticleSystem* ps, vector<float>* dst)
{
	ps->ClearForces();
	ps->CalculateForces(); // Later Going to be Initialized

	for (Particle* p : ps->Particles)
	{
		dst->push_back(p->v[0]);
		dst->push_back(p->v[1]);
		dst->push_back(p->v[2]);
		dst->push_back(p->f[0] / p->m);
		dst->push_back(p->f[1] / p->m);
		dst->push_back(p->f[2] / p->m);
	}
}

void ScaleUp(vector<float>* v1, vector<float>* v2, float h)
{
	for(int i=0; i<int(v1->size()); ++i)
	{
		v1->at(i) = v1->at(i) + v2->at(i)*h;
	}
}

void EulerSolver(ParticleSystem* ps, float DeltaT)
{
	GetParticleState(ps, &temp1);
	ParticleDerivative(ps, &temp2);
	ScaleUp(&temp1, &temp2, DeltaT);
	SetParticleState(ps, &temp1);
	ps->time += DeltaT;
}

// Create Solver Class 

/*
int main()
{
	float mass{ 1.0 };
	array<float, 3> pos{ 1,1,1 };
	array<float, 3> vel{ 2,2,2 };
	array<float, 3> force{ 3,3,3 };

	Particle p(1.0, &pos, &vel, &force);

	ParticleSystem ps;

	ps.AddParticle(&p);

	std::cout << p.f[0] << p.f[1] << p.f[2];

	return 0;
}
*/