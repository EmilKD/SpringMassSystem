#ifndef PDB_H
#define PDB_H

#include<array>
#include<vector>

using std::vector;
using std::array;

// Objects ------------------------------------------------
struct Particle
{
	int ID{ 0 };
	float m{ 0 };
	array<float, 3> p;
	array<float, 3> v;
	array<float, 3> f;

	Particle(int pid = 0, float mass = 1.0f, array<float, 3>* position = NULL, array<float, 3>* velocity = NULL, array<float, 3>* force = NULL)
	{
		ID = pid;
		m = mass;
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

class ParticleSystem
{
public:
	ParticleSystem();
	void AddParticle(Particle* p);
	void ClearForces();
	void CalculateForces();
	//void SetParticleStates(const ParticleSystem* ps, vector<float>* src);
	//void GetParticleStates(const ParticleSystem* ps, vector<float>* dst);
	//void ParticleDerivative(ParticleSystem ps, array<float, 6> dst);

	vector<Particle> Particles{};
	int n{ 0 };
	float time{ 0.0f };
	float Gravity{ 9.8f };
	float Drag{ 0.1f };

private:

};

#endif // !PDB_H