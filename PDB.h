#ifndef PDB_H
#define PDB_H

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/norm.hpp>
#include<array>
#include<vector>
#include<iostream>

using std::vector;
using std::array;

// Objects ------------------------------------------------
struct Particle
{
	int ID{ 0 };
	float m{ 0 };
	glm::vec3 p{};
	glm::vec3 v{};
	glm::vec3 f{};

	Particle(int pid = 0, float mass = 1.0f, glm::vec3* position = NULL, glm::vec3* velocity = NULL, glm::vec3* force = NULL) : ID{pid}, m{mass}
	{
		if (position)
			p = *position;
		else
			p = { 0, 0, 0 };
		if(velocity)
			v = *velocity;
		else
			v = { 0, 0, 0 };
		if(force)
			f = *force;
		else
			f = { 0, 0, 0 };
	}
};

class SpringConstraint;

class ParticleSystem
{
public:
	ParticleSystem();
	void AddParticle(Particle* p);
	void DeleteParticle(unsigned int id);
	void ClearForces();
	void CalculateForces();
	int ParticleDim();
	void SetParticleState(vector<float>* src);
	void GetParticleState(vector<float>* dst) const;
	void ParticleDerivative(vector<float>* dst);
	void ScaleUp(vector<float>* v1, vector<float>* v2, float h);
	void CollisionDetect();
	void EulerSolve(float DeltaT);
	void BackwardsEulerSolve(float DeltaT);

	vector<Particle> Particles{};
	int n{ 0 };
	float time{ 0.0f };
	float Gravity{ -9.83f };
	float Drag{ 0.1f };
	float repulsion{ 100.0f };
	float repulsionRadius{ 2 };

	vector<SpringConstraint> sConstraints{};

	vector<int> SpringParticles{}; // 
	vector<float> SpringConsts{}; // <float ks, float kd, float l0, ... >
	vector<int> Ignoreparticles{};

private:

};

// Force Objects -----------------------------------------------------------------------------------------------

class SpringConstraint
{
public:
	SpringConstraint(unsigned int id, Particle* p1, Particle* p2, float kstiff = 1000.0f, float kdamp = 100.0f, float l0 = 4.0f) : ID{id}, ks { kstiff }, kd{ kdamp }, particleIDs{ p1->ID, p2->ID }
	{
		glm::vec3 pos12 = p1->p - p2->p;
		float pmag = glm::length(pos12);
		restLength = pmag;
	}

	void CalculateSpringForce(ParticleSystem* ps)
	{
		static int counter{ 0 };
		glm::vec3 pos12 = ps->Particles[particleIDs[0]].p - ps->Particles[particleIDs[1]].p;
		glm::vec3 pos12dir = glm::normalize(pos12);
		float pmag = glm::length(pos12);

		glm::vec3 vel12 = ps->Particles[particleIDs[0]].v - ps->Particles[particleIDs[1]].v;

		glm::vec3 spring_force = (ks * (pmag - restLength) + kd * glm::dot(vel12, pos12) / pmag) * pos12dir;

		float forceMag = glm::length(spring_force);

		/*if (counter > 1000)
		{
			std::cout << "rest length: " << restLength << std::endl;
			counter = 0;
		}*/

		counter++;
		if (forceMag > PlasticityForce && (pmag - restLength) > 0)
		{
			//std::cout << "plastic tensile deformation!" << std::endl;
			ps->sConstraints[this->ID].restLength *= 1.01;
			ps->sConstraints[this->ID].PlasticityForce *= 1.01;
		}
		else if(forceMag > PlasticityForce && (pmag - restLength) < 0)
		{
			//std::cout << "plastic compressive deformation!" << std::endl;
			ps->sConstraints[this->ID].restLength *= 0.99;
			ps->sConstraints[this->ID].PlasticityForce *= 1.01;
		}

		// derivetives for backwards euler solve -----------------------

		//glm::mat3 dfdxi = -ks * ((1 - l0 / pmag) * (glm::mat3(1.0f) - glm::outerProduct(pos12dir, pos12dir)) + glm::outerProduct(pos12dir, pos12dir));
		//glm::mat3 dfdxj = -dfdxi;
		//glm::mat3 dfdv = -kd * glm::mat3(1.0f);

		ps->Particles[particleIDs[0]].f -= spring_force;
		ps->Particles[particleIDs[1]].f += spring_force;
	}

	float getRestLength()
	{
		return restLength;
	}

	void setRestLength(float newLength)
	{
		restLength = newLength;
	}

	array<int, 2> getParticleIDs()
	{
		return particleIDs;
	}

private:
	unsigned int ID{};
	float ks;
	float kd;
	float restLength;
	float PlasticityForce{1000.0f};
	array<int, 2> particleIDs;
};

#endif // !PDB_H