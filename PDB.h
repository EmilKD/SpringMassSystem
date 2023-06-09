#ifndef PDB_H
#define PDB_H

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/norm.hpp>
#include<array>
#include<vector>

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

	Particle(int pid = 0, float mass = 1.0f, glm::vec3* position = NULL, glm::vec3* velocity = NULL, glm::vec3* force = NULL)
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
	float Gravity{ -9.83f };
	float Drag{ 0.01f };

	vector<array<int, 2>> SpringParticles{};
	vector<array<float, 2>> SpringConsts{};
	vector<float> SpringLengths{};

private:

};

#endif // !PDB_H