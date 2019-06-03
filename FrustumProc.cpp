#include "include/FrustumProc.hpp"

FrustumProc::FrustumProc(Planet *planet, Camera *camera)
{
  this->planet = planet;
  this->camera = camera;
  this->frustum = new Frustum();
}

void FrustumProc::calculateFrustum()
{
    frustum->SetCullTransform(glm::mat4(1.0f)); //inserir matriz de transformação
    frustum->SetToCamera(this->camera);

    //frustum->Transform(space);
    frustum->Update();
    frustum->ContainsQuad(this->planet->getCube()->Back);
    frustum->ContainsQuad(this->planet->getCube()->Bottom);
    frustum->ContainsQuad(this->planet->getCube()->Front);
    frustum->ContainsQuad(this->planet->getCube()->Left);
    frustum->ContainsQuad(this->planet->getCube()->Right);
    frustum->ContainsQuad(this->planet->getCube()->Top);
}
