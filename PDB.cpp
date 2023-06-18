#include "PDB.h"

bool CheckExist(vector<int> base, int value)
{
	for(int i=0; i<base.size(); ++i)
	{
		if (base[i] == value)
			return true;
	}
	return false;
}

// Force Objects -----------------------------------------------------------------------------------------------
void SpringForce(Particle* p1, Particle* p2, float ks = 5000.0f, float kd = 10.0f, float l0 = 4.0f)
{
	glm::vec3 pos12 = p1->p - p2->p;
	float pmag = glm::length(pos12);

	glm::vec3 vel12 = p1->v - p2->v;

	glm::vec3 spring_force = (ks * (pmag - l0) + kd * glm::dot(vel12, pos12)/pmag) * glm::normalize(pos12);

	// Print out the forces
	//std::cout << glm::length(spring_force) << std::endl;

	p1->f -= spring_force;
	p2->f += spring_force;
}



// Particle System Functions ----------------------------------------------------------------------------------
ParticleSystem::ParticleSystem()
{
	this->n = int(Particles.size());
}

void ParticleSystem::AddParticle(Particle* p)
{
	this->Particles.push_back(*p);
	this->n = this->Particles.size();
}

void ParticleSystem::DeleteParticle(unsigned int id)
{
	if (this->n > 0)
	{
		// Deleting Particles from the Particle System
		this->Particles.erase(Particles.begin() + id);

		// Removing Deleted Spring Particles IDs
		for (int i = 0; i < this->SpringParticles.size(); ++i)
		{
			if (this->SpringParticles[i] == id)
			{
				this->SpringParticles.erase(this->SpringParticles.begin() + i);
				this->SpringParticles.erase(this->SpringParticles.begin() + i);
			}
		}

		this->n = this->Particles.size();
	}
	
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
	for (int i{ 0 }; i < Particles.size(); ++i)
	{
		Particles[i].f[1] += Gravity - Drag * Particles[i].v[1];

		// if particle has string
	}
	
	for (int i{ 0 }; i<SpringParticles.size()/2; ++i)
	{
		SpringForce(&Particles[SpringParticles[2*i]],
			&Particles[SpringParticles[2*i + 1]]);
	}
	
}


// ODE Solver --------------------------------------------------------------------------------------------------------------------------------------
int ParticleSystem::ParticleDim()
{
	return 6 * this->n;
}

void ParticleSystem::GetParticleState(vector<float>* dst) const
{
	for(Particle p: this->Particles)
	{
		dst->push_back(p.p[0]);
		dst->push_back(p.p[1]);
		dst->push_back(p.p[2]);
		dst->push_back(p.v[0]);
		dst->push_back(p.v[1]);
		dst->push_back(p.v[2]);
	}
}

void ParticleSystem::SetParticleState(vector<float>* src)
{
	for (int i = 0; i < this->n; ++i)
	{
		if (!CheckExist(this->Ignoreparticles, i))
		{
			this->Particles[i].p = { src->at(6 * i), src->at(6 * i + 1), src->at(6 * i + 2) };
			this->Particles[i].v = { src->at(6 * i + 3), src->at(6 * i + 4), src->at(6 * i + 5) };
		}
	}
}

void ParticleSystem::ParticleDerivative(vector<float>* dst)
{
	this->ClearForces();
	this->CalculateForces();

	for (Particle p : this->Particles)
	{
		dst->push_back(p.v[0]);
		dst->push_back(p.v[1]);
		dst->push_back(p.v[2]);
		dst->push_back(p.f[0] / p.m);
		dst->push_back(p.f[1] / p.m);
		dst->push_back(p.f[2] / p.m);
	}
}

void ParticleSystem::ScaleUp(vector<float>* v1, vector<float>* v2, float h)
{
		for (int i = 0; i<int(v1->size()); ++i)
		{
			v1->at(i) = v1->at(i) + v2->at(i) * h;
		}
}

void ParticleSystem::CollisionDetect()
{
	int counter = 0;
	float r = 0.5;

	array<float, 2> bounds{15.0f, 15.0f};


	for(Particle p: this->Particles)
	{

		if (p.p[0] >= bounds[0] || p.p[0] <= -bounds[0])
		{
			this->Particles[counter].p[0] = bounds[0] * glm::sign(p.p[0]);
			this->Particles[counter].v[0] = -p.v[0]*r;
			this->Particles[counter].f[0] -= p.f[0];
		}
		if (p.p[1] >= bounds[1] || p.p[1] <= -bounds[1])
		{
			this->Particles[counter].p[1] = bounds[1] * glm::sign(p.p[1]);
			this->Particles[counter].v[1] = -p.v[1]*r;
			this->Particles[counter].f[1] -= p.f[1];
		}
		++counter;
	}
}

void ParticleSystem::EulerSolve(float DeltaT)
{
	vector<float> temp1{};
	vector<float> temp2{};
	GetParticleState(&temp1);
	ParticleDerivative(&temp2);
	ScaleUp(&temp1, &temp2, DeltaT);
	SetParticleState(&temp1);
	CollisionDetect();
	this->time += DeltaT;
}

