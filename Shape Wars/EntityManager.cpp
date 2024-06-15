#include "EntityManager.h"
#include <algorithm>

EntityManager::EntityManager(){}

void EntityManager::update()
{
	for (auto& e : m_toAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}
	m_toAdd.clear();

	// remove dead entities from the vector of all entities
	removeDeadEntities(m_entities);

	 // remove dead entites from each vector in the entity map
	for (auto& pair : m_entityMap)
	{
		removeDeadEntities(pair.second);
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag) 
{
	auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	m_toAdd.push_back(e);
	return e;
}

EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}


void EntityManager::removeDeadEntities(EntityVec& vec)
{
	// is_inactive is a lamda function that that checks if an entity is dead.
	// This funtion is required in the std::remove_if function as the third argument
	// because the function has to take in one argument (in this cases the entity) and return a bool value 
	// which will determine if it should be removed or not
	auto is_inactive = [](const std::shared_ptr<Entity>& e) { return !e->isActive(); };

	//std::remove_if: This algorithm reorders the elements in the range [vec.begin(), vec.end()]
	// such that all the elements that do not satisfy the is_inactive condition appear before those that need to be removed
	// vec.erase: This erases the elements that are inactive from the vector.
	vec.erase(std::remove_if(vec.begin(), vec.end(), is_inactive), vec.end());
}
