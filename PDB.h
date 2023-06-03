#ifndef PDB_H
#define PDB_H

#include<array>
#include<vector>

using std::vector;
using std::array;

// Objects ------------------------------------------------
struct Particle
{
	float m{ 0 };
	array<float, 3> p;
	array<float, 3> v;
	array<float, 3> f;

	Particle(float mass, array<float, 3>* position, array<float, 3>* vellocity, array<float, 3>* force)
	{
		m = mass;
		p = *position;
		v = *vellocity;
		f = *force;
	}
};

class ParticleSystem
{
public:
	ParticleSystem();
	void AddParticle(Particle* p);
	void ClearForces();
	void CalculateForces();
	void SetParticleStates();
	void GetParticleStates();
	void ParticleDerivative(ParticleSystem ps, array<float, 6> dst);

	vector<Particle*> Particles{};
	int n{ 0 };
	float time{ 0.0f };
	float Gravity{ 9.8f };
	float Drag{ 0.1f };

private:

};

#endif // !PDB_H