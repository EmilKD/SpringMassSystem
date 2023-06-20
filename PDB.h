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

class SpringConstraint
{
public:
	SpringConstraint(Particle* sp1, Particle* sp2);
	void CalculateConstraintForce();
	void setRestLength(float newLength);
	float getRestLength();

	array<Particle*, 2> SpringParticles{};

private:
	float ks{ 10.0f };
	float kd{ 1.0f };
	float RestLength{ 4.0f };
};


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

	vector<Particle> Particles{};
	int n{ 0 };
	float time{ 0.0f };
	float Gravity{ -9.83f };
	float Drag{ 0.1f };

	vector<SpringConstraint> sConstraint{};
	vector<int> SpringParticles{};
	vector<float> SpringConsts{};
	vector<float> SpringLengths{};
	vector<int> Ignoreparticles{};

private:

};



#endif // !PDB_H