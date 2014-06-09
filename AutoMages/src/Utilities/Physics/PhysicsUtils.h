/*
 * PhysicsUtils.h
 *
 *  Created on: Dec 13, 2012
 *      Author: manicqin
 */

#ifndef PHYSICSUTILS_H_
#define PHYSICSUTILS_H_

#include <memory>
#include <btBulletDynamicsCommon.h>
#include <Ogre.h>
//Copy paste from PhysicsBullet Ogre example
//It will auto update Ogre when moving.

class MotionStateBltOgre : public btMotionState {
public:
	MotionStateBltOgre(const btTransform &initialpos, std::shared_ptr<Ogre::SceneNode> node) {
        mVisibleobj = node;
        mPos1 = initialpos;
    }

    virtual ~MotionStateBltOgre() {
    }

    void setNode(std::shared_ptr<Ogre::SceneNode> node) {
        mVisibleobj = node;
    }

    virtual void getWorldTransform(btTransform &worldTrans) const {
        worldTrans = mPos1;
    }

    virtual void setWorldTransform(const btTransform &worldTrans) {
        if(NULL == mVisibleobj)
            return; // silently return before we set a node
        btQuaternion rot = worldTrans.getRotation();
        mVisibleobj->setOrientation(rot.w(), rot.x(), rot.y(), rot.z());
        btVector3 pos = worldTrans.getOrigin();
        mVisibleobj->setPosition(pos.x(), pos.y(), pos.z());
    }

protected:
    std::shared_ptr<Ogre::SceneNode> mVisibleobj;
    btTransform mPos1;
};

#endif /* PHYSICSUTILS_H_ */
