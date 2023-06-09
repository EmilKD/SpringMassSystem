#include "PDB.h"

// Force Objects -----------------------------------------------------------------------------------------------
void SpringForce(Particle* p1, Particle* p2, float ks = 10.0f, float kd = 0.01f, float l0 = 0.5f)
{
	glm::vec3 pos12 = p1->p - p2->p;
	float pmag = glm::length(pos12);

	glm::vec3 vel12 = p1->v - p2->v;

	glm::vec3 spring_force = (ks * abs((pmag - l0)) + kd * glm::dot(vel12, pos12)/pmag) * glm::normalize(pos12);

	// Print out the forces
	//std::cout << glm::length(spring_force) << std::endl;

	p1->f -= spring_force;
	p2->f += spring_force;
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

	// For Loops could be optimized by turing into matrix operations 
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
		dst->push_back(p.v[2]);
	}
}

void SetParticleState(ParticleSystem* ps, vector<float>* src)
{
	for (int i = 1; i < ps->n; ++i)
	{
		ps->Particles[i].p = { src->at(6 * i), src->at(6 * i + 1), src->at(6 * i + 2) };
		ps->Particles[i].v = { src->at(6 * i + 3), src->at(6 * i + 4), src->at(6 * i + 5) };
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
