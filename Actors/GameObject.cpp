#include "GameObject.hpp"

GameObject::GameObject() {

}

GameObject::~GameObject() {

}

void GameObject::Update() {
	mPosition = mSprite.mVertexData.Position;
}