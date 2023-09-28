#include "ModelManager.h"

ModelManager::ModelManager(){

}

ModelManager::~ModelManager() {

}

void ModelManager::AddModel(Model* m) {
	_models.push_back(m);
}

void ModelManager::deleteModel(Model* m) {
	vector<Model*>::iterator it = find(_models.begin(), _models.end(), m);

	if (it != _models.end()) {
		_models.erase(it);
	}
}

void ModelManager::Draw() {
	for (auto m : _models) {

	}
}