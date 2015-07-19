/*******************************************************************************
* Copyright 2011 See AUTHORS file.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
******************************************************************************/
/** @author Xoppa */
#ifdef _MSC_VER
#pragma once
#endif //_MSC_VER
#ifndef FBXCONV_READERS_FBXANIMATION_H
#define FBXCONV_READERS_FBXANIMATION_H

#include <fbxsdk.h>
#include "../Settings.h"
#include "../log/log.h"
#include "util.h"
#include "../modeldata/Model.h"
#include <map>

using namespace fbxconv::modeldata;

namespace fbxconv {
namespace readers {

	class FbxAnimation {
	public:
		Settings *settings;
		fbxconv::log::Log *log;
		Model * const model;
		std::map<const FbxNode *, Node *> nodeMap;

		FbxAnimation(Settings *settings, fbxconv::log::Log *log, Model * const &model, std::map<const FbxNode *, Node *> const &nodeMap)
			: settings(settings), log(log), model(model), nodeMap(nodeMap)
		{

		}

		inline static const char *getInterpolationName(FbxAnimCurveDef::EInterpolationType iType) {
			switch (iType) {
			case FbxAnimCurveDef::eInterpolationConstant: return "constant";
			case FbxAnimCurveDef::eInterpolationLinear: return "linear";
			case FbxAnimCurveDef::eInterpolationCubic: return "cubic";
			}
			return "unknown";
		}

		static const unsigned short PropTranslation = 1;
		static const unsigned short PropRotation = 2;
		static const unsigned short PropScaling = 3;
		unsigned short getPropertyType(FbxAnimStack * const &animStack, FbxProperty const &prop, FbxNode * &node) {
			node = static_cast<FbxNode *>(prop.GetFbxObject());
			FbxString propName = prop.GetName();
			if (propName.Compare("DeformPercent") != 0)
			{
				if (node->LclTranslation.IsValid() && propName.Compare(node->LclTranslation.GetName()) == 0)
					return PropTranslation;
				if (node->LclRotation.IsValid() && propName.Compare(node->LclRotation.GetName()) == 0)
					return PropRotation;
				if (node->LclScaling.IsValid() && propName.Compare(node->LclScaling.GetName()) == 0)
					return PropScaling;
			}
			log->warning(log::wSourceConvertFbxSkipPropname, animStack->GetName(), (const char *)propName);
			return 0;
		}

		Animation *convert(FbxAnimStack * const &animStack) {
			Animation *result = 0;
			if (settings->forceFpsSamplesAnimations || !convertAnimation(animStack, result)) {
				log->warning(log::wSourceConvertFbxFallbackAnimation, animStack->GetName());
				convertAnimationBySampling(animStack, result);
			}
			return result;
		}

		inline static int indexOfNodeAnimation(Animation * const &animation, Node * const &node) {
			int index = 0;
			for (std::vector<NodeAnimation *>::const_iterator it = animation->nodeAnimations.begin(); it != animation->nodeAnimations.end(); ++it, ++index) {
				if ((*it)->node == node)
					return index;
			}
			return -1;
		}

		inline static int addNodeAnimationIfNotEmpty(Animation * const &animation, NodeAnimation * const &nodeAnimation, bool const &deleteIfEmpty) {
			if (nodeAnimation) {
				if ((nodeAnimation->rotate && !nodeAnimation->rotation.empty()) ||
					(nodeAnimation->translate && !nodeAnimation->translation.empty()) ||
					(nodeAnimation->scale && !nodeAnimation->scaling.empty())) {
					animation->nodeAnimations.push_back(nodeAnimation);
					return animation->nodeAnimations.size() - 1;
				}
				if (deleteIfEmpty)
					delete nodeAnimation;
			}
			return -1;
		}

		template<int n>
		void cleanupKeyframes(std::vector<Keyframe<n>> &keyframes, float *defaultValue) {
			for (int i = 1; i < (keyframes.size() - 1);) {
				Keyframe<n> &k1 = keyframes[i - 1];
				Keyframe<n> &k2 = keyframes[i];
				Keyframe<n> &k3 = keyframes[i + 1];
				if (isLerp(k1.value, k1.time, k2.value, k2.time, k3.value, k3.time, n))
					keyframes.erase(keyframes.begin() + i);
				else
					++i;
			}
			if ((keyframes.size() == 2) && (cmp(keyframes[0].value, keyframes[1].value, n)))
				keyframes.erase(keyframes.begin() + 1);
			if ((keyframes.size() == 1) && (cmp(keyframes[0].value, defaultValue, n)))
				keyframes.clear();
		}

		bool addTranslationKeyframes(NodeAnimation * const &nodeAnimation, FbxNode *node, FbxAnimCurve * const &curve) {
			assert(nodeAnimation->translation.empty());
			int keyCount = curve->KeyGetCount();
			for (int keyIndex = 0; keyIndex < keyCount; ++keyIndex) {
				FbxTime keyTime = curve->KeyGetTime(keyIndex);
				FbxVector4 localTranslation = node->EvaluateLocalTranslation(keyTime);
				Keyframe<3> kf;
				kf.time = (float)(1000.0 * keyTime.GetSecondDouble());
				kf.value[0] = localTranslation.mData[0];
				kf.value[1] = localTranslation.mData[1];
				kf.value[2] = localTranslation.mData[2];
				nodeAnimation->translation.push_back(kf);
			}
			cleanupKeyframes<3>(nodeAnimation->translation, (float*)nodeAnimation->node->transform.translation);
			nodeAnimation->translate = !nodeAnimation->translation.empty();
			return true;
		}

		bool addRotationKeyframes(NodeAnimation * const &nodeAnimation, FbxNode *node, FbxAnimCurve * const &curve) {
			assert(nodeAnimation->rotation.empty());
			int keyCount = curve->KeyGetCount();
			for (int keyIndex = 0; keyIndex < keyCount; ++keyIndex) {
				FbxTime keyTime = curve->KeyGetTime(keyIndex);
				FbxAMatrix tmp;
				tmp.SetR(node->EvaluateLocalRotation(keyTime));
				FbxQuaternion localRotation = tmp.GetQ();
				Keyframe<4> kf;
				kf.time = (float)(1000.0 * keyTime.GetSecondDouble());
				kf.value[0] = localRotation.mData[0];
				kf.value[1] = localRotation.mData[1];
				kf.value[2] = localRotation.mData[2];
				kf.value[3] = localRotation.mData[3];
				nodeAnimation->rotation.push_back(kf);
			}
			int start = nodeAnimation->rotation.size();
			cleanupKeyframes<4>(nodeAnimation->rotation, (float*)nodeAnimation->node->transform.rotation);
			int end = nodeAnimation->rotation.size();
			if (end != start)
				printf("Removed %d - %d = %d keyframes\n", start, end, start - end);
			nodeAnimation->rotate = !nodeAnimation->rotation.empty();
			return true;
		}

		/** Add the specified animation by adding the actual keyframes of components the animation consist of */
		bool convertAnimation(FbxAnimStack * const &animStack, Animation * &result) {
			const int layerCount = animStack->GetMemberCount<FbxAnimLayer>();
			if (layerCount != 1) {
				log->warning(log::wSourceConvertFbxLayeredAnimation, animStack->GetName(), layerCount);
				return false;
			}

			bool success = true;
			Animation *animation = new Animation();
			animation->id = animStack->GetName();
			animStack->GetScene()->SetCurrentAnimationStack(animStack);

			// There should only be one layer (layers are used for blending animations)
			for (int layerIndex = 0; success && layerIndex < layerCount; ++layerIndex) {
				FbxAnimLayer *layer = animStack->GetMember<FbxAnimLayer>(layerIndex);
				Node *node = 0;
				NodeAnimation *nodeAnimation = 0;
				int nodeAnimationIndex = -1;
				const int curveNodeCount = layer->GetSrcObjectCount<FbxAnimCurveNode>();
				// Each node affected by this animation (although the fbx design doesnt seem to restrict it to a single node, which is why the node is fetched per property)
				for (int curveNodeIndex = 0; success && curveNodeIndex < curveNodeCount; ++curveNodeIndex) {
					FbxAnimCurveNode *curveNode = layer->GetSrcObject<FbxAnimCurveNode>(curveNodeIndex);

					const int curveCount = curveNode->GetCurveCount(0U);
					if (curveCount != 1) {
						log->warning(log::wSourceConvertFbxMultipleCurves, animStack->GetName(), curveCount);
						success = false;
						break;
					}

					FbxAnimCurve *curve = curveNode->GetCurve(0U);
					FbxAnimCurveDef::EInterpolationType itype = curve->KeyGetInterpolation(0);

					unsigned short prop;
					FbxNode *fbxNode = 0;
					const int propertyCount = curveNode->GetDstPropertyCount();
					// Each property of this node affected (keyed) by this animation
					for (int propertyIndex = 0; success && propertyIndex < propertyCount; ++propertyIndex) {
						if (!(prop = getPropertyType(animStack, curveNode->GetDstProperty(propertyIndex), fbxNode)))
							continue;
						std::map<const FbxNode *, Node *>::iterator it = nodeMap.find(fbxNode);
						if (it == nodeMap.end()) {
							log->warning(log::wSourceConvertFbxUnknownNodeAnim, animStack->GetName(), fbxNode->GetName());
							continue;
						}
						if (node != it->second) {
							if (nodeAnimationIndex < 0)
								addNodeAnimationIfNotEmpty(animation, nodeAnimation, true);
							node = it->second;
							nodeAnimationIndex = indexOfNodeAnimation(animation, node);
							if (nodeAnimationIndex < 0)
								(nodeAnimation = new NodeAnimation())->node = node;
							else
								nodeAnimation = animation->nodeAnimations.at(nodeAnimationIndex);
						}

						if (prop == PropTranslation)
							success = success && addTranslationKeyframes(nodeAnimation, fbxNode, curve);
						else if (prop == PropRotation)
							success = success && addRotationKeyframes(nodeAnimation, fbxNode, curve);

						//printf("Anim(%s).Layer(%s).CurveNode(%s).Node(%s).Prop(%d).Keys[%d] = %s\n", animStack->GetName(), layer->GetName(), curveNode->GetName(), node->id.c_str(), prop, keyCount, getInterpolationName(itype));
					}
				}
				if (nodeAnimationIndex < 0)
					addNodeAnimationIfNotEmpty(animation, nodeAnimation, true);
			}

			if (!success)
				delete animation;
			else if (animation->nodeAnimations.empty()) {
				log->warning(log::wSourceConvertFbxSkipAnimation, animation->id.c_str());
				delete animation;
			}
			else
				result = animation;

			return success;
		}


		////////////////////////////////////////////////////////////////
		//// OLDER (v0.1) SAMPLING BASED ANIMATIONS BELOW THIS LINE ////
		////////////////////////////////////////////////////////////////

		/** Add the specified animation to the model by samples the affected nodes transforms for each keyframe */
		void convertAnimationBySampling(FbxAnimStack * const &animStack, Animation * &animation) {
			static std::vector<Keyframe<3>> translationKeyFrames;
			static std::vector<Keyframe<4>> rotationKeyFrames;
			static std::vector<Keyframe<3>> scalingKeyFrames;
			static std::map<FbxNode *, AnimInfo> affectedNodes;
			affectedNodes.clear();

			FbxTimeSpan animTimeSpan = animStack->GetLocalTimeSpan();
			float animStart = (float)(animTimeSpan.GetStart().GetMilliSeconds());
			float animStop = (float)(animTimeSpan.GetStop().GetMilliSeconds());
			if (animStop <= animStart)
				animStop = 999999999.0f;

			// Could also use animStack->GetLocalTimeSpan and animStack->BakeLayers, but its not guaranteed to be correct
			const int layerCount = animStack->GetMemberCount<FbxAnimLayer>();
			for (int l = 0; l < layerCount; l++) {
				FbxAnimLayer *layer = animStack->GetMember<FbxAnimLayer>(l);
				// For each layer check which node is affected and within what time frame and rate
				const int curveNodeCount = layer->GetSrcObjectCount<FbxAnimCurveNode>();
				for (int n = 0; n < curveNodeCount; n++) {
					FbxAnimCurveNode *curveNode = layer->GetSrcObject<FbxAnimCurveNode>(n);
					// Check which properties on this curve are changed
					const int nc = curveNode->GetDstPropertyCount();
					for (int o = 0; o < nc; o++) {
						FbxProperty prop = curveNode->GetDstProperty(o);
						FbxNode *node = static_cast<FbxNode *>(prop.GetFbxObject());
						if (node) {
							FbxString propName = prop.GetName();
							if (propName == "DeformPercent")
							{
								// When using this propName in model an unhandled exception is launched in sentence node->LclTranslation.GetName()
								log->warning(log::wSourceConvertFbxSkipPropname, animStack->GetName(), (const char *)propName);
								continue;
							}

							// Only add translation, scaling or rotation
							if ((!node->LclTranslation.IsValid() || propName != node->LclTranslation.GetName()) &&
								(!node->LclScaling.IsValid() || propName != node->LclScaling.GetName()) &&
								(!node->LclRotation.IsValid() || propName != node->LclRotation.GetName()))
								continue;
							FbxAnimCurve *curve;
							AnimInfo ts;
							ts.translate = propName == node->LclTranslation.GetName();
							ts.rotate = propName == node->LclRotation.GetName();
							ts.scale = propName == node->LclScaling.GetName();
							if (curve = prop.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X))
								updateAnimTime(curve, ts, animStart, animStop);
							if (curve = prop.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y))
								updateAnimTime(curve, ts, animStart, animStop);
							if (curve = prop.GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z))
								updateAnimTime(curve, ts, animStart, animStop);
							//if (ts.start < ts.stop)
							affectedNodes[node] += ts;
						}
					}
				}
			}

			if (affectedNodes.empty())
				return;

			animation = new Animation();
			animation->id = animStack->GetName();
			animStack->GetScene()->SetCurrentAnimationStack(animStack);

			// Add the NodeAnimations to the Animation
			for (std::map<FbxNode *, AnimInfo>::const_iterator itr = affectedNodes.begin(); itr != affectedNodes.end(); itr++) {
				Node *node = model->getNode((*itr).first->GetName());
				if (!node)
					continue;
				translationKeyFrames.clear();
				rotationKeyFrames.clear();
				scalingKeyFrames.clear();
				NodeAnimation *nodeAnim = new NodeAnimation();
				nodeAnim->node = node;
				nodeAnim->translate = (*itr).second.translate;
				nodeAnim->rotate = (*itr).second.rotate;
				nodeAnim->scale = (*itr).second.scale;
				const float stepSize = (*itr).second.framerate <= 0.f ? (*itr).second.stop - (*itr).second.start : 1000.f / (*itr).second.framerate;
				const float last = (*itr).second.stop + stepSize * 0.5f;
				FbxTime fbxTime;
				// Calculate all keyframes upfront
				for (float time = (*itr).second.start; time <= last; time += stepSize) {
					time = std::min(time, (*itr).second.stop);
					fbxTime.SetMilliSeconds((FbxLongLong)time);
					Keyframe<3> kfT;
					Keyframe<4> kfR;
					Keyframe<3> kfS;
					kfT.time = (time - animStart);
					kfR.time = (time - animStart);
					kfS.time = (time - animStart);
					FbxAMatrix *m = &(*itr).first->EvaluateLocalTransform(fbxTime);
					FbxVector4 v = m->GetT();
					kfT.value[0] = (float)v.mData[0];
					kfT.value[1] = (float)v.mData[1];
					kfT.value[2] = (float)v.mData[2];
					FbxQuaternion q = m->GetQ();
					kfR.value[0] = (float)q.mData[0];
					kfR.value[1] = (float)q.mData[1];
					kfR.value[2] = (float)q.mData[2];
					kfR.value[3] = (float)q.mData[3];
					v = m->GetS();
					kfS.value[0] = (float)v.mData[0];
					kfS.value[1] = (float)v.mData[1];
					kfS.value[2] = (float)v.mData[2];
					translationKeyFrames.push_back(kfT);
					rotationKeyFrames.push_back(kfR);
					scalingKeyFrames.push_back(kfS);
				}
				// Only add keyframes really needed
				addKeyframes(nodeAnim, translationKeyFrames, rotationKeyFrames, scalingKeyFrames);
				if (nodeAnim->rotate || nodeAnim->scale || nodeAnim->translate)
					animation->nodeAnimations.push_back(nodeAnim);
				else
					delete nodeAnim;
			}
		}

		inline void updateAnimTime(FbxAnimCurve *const &curve, AnimInfo &ts, const float &animStart, const float &animStop) {
			FbxTimeSpan fts;
			curve->GetTimeInterval(fts);
			const FbxTime start = fts.GetStart();
			const FbxTime stop = fts.GetStop();
			ts.start = std::max(animStart, std::min(ts.start, (float)(start.GetMilliSeconds())));
			ts.stop = std::min(animStop, std::max(ts.stop, (float)stop.GetMilliSeconds()));
			// Could check the number and type of keys (ie curve->KeyGetInterpolation) to lower the framerate
			ts.framerate = std::max(ts.framerate, (float)stop.GetFrameRate(FbxTime::eDefaultMode));
		}

		void addKeyframes(NodeAnimation *const &anim, std::vector<Keyframe<3>> &translationKeyFrames, std::vector<Keyframe<4>> &rotationKeyFrames, std::vector<Keyframe<3>> &scalingKeyFrames) {
			bool translate = false, rotate = false, scale = false;
			// Check which components are actually changed
			for (std::vector<Keyframe<3>>::const_iterator itr = translationKeyFrames.begin(); itr != translationKeyFrames.end(); ++itr) {
				if (!translate && !cmp(anim->node->transform.translation, itr->value, 3))
					translate = true;
			}
			for (std::vector<Keyframe<4>>::const_iterator itr = rotationKeyFrames.begin(); itr != rotationKeyFrames.end(); ++itr) {
				if (!rotate && !cmp(anim->node->transform.rotation, itr->value, 4))
					rotate = true;
			}
			for (std::vector<Keyframe<3>>::const_iterator itr = scalingKeyFrames.begin(); itr != scalingKeyFrames.end(); ++itr) {
				if (!scale && !cmp(anim->node->transform.scale, itr->value, 3))
					scale = true;
			}
			// This allows to only export the values actual needed
			anim->translate = translate;
			anim->rotate = rotate;
			anim->scale = scale;

			if (translate && !translationKeyFrames.empty()) {
				anim->translation.push_back(translationKeyFrames[0]);
				const int last = (int)translationKeyFrames.size() - 1;
				Keyframe<3> k1 = translationKeyFrames[0], k2, k3;
				for (int i = 1; i < last; i++) {
					k2 = translationKeyFrames[i];
					k3 = translationKeyFrames[i + 1];
					// Check if the middle keyframe can be calculated by information, if so dont add it
					if (translate && !isLerp(k1.value, k1.time, k2.value, k2.time, k3.value, k3.time, 3)) {
						anim->translation.push_back(k2);
						k1 = k2;
					}
				}
				if (last > 0)
					anim->translation.push_back(translationKeyFrames[last]);
			}

			if (rotate && !rotationKeyFrames.empty()) {
				anim->rotation.push_back(rotationKeyFrames[0]);
				const int last = (int)rotationKeyFrames.size() - 1;
				Keyframe<4> k1 = rotationKeyFrames[0], k2, k3;
				for (int i = 1; i < last; i++) {
					k2 = rotationKeyFrames[i];
					k3 = rotationKeyFrames[i + 1];
					// Check if the middle keyframe can be calculated by information, if so dont add it
					if (rotate && !isLerp(k1.value, k1.time, k2.value, k2.time, k3.value, k3.time, 4)) {
						anim->rotation.push_back(k2);
						k1 = k2;
					}
				}
				if (last > 0)
					anim->rotation.push_back(rotationKeyFrames[last]);
			}

			if (scale && !scalingKeyFrames.empty()) {
				anim->scaling.push_back(scalingKeyFrames[0]);
				const int last = (int)scalingKeyFrames.size() - 1;
				Keyframe<3> k1 = scalingKeyFrames[0], k2, k3;
				for (int i = 1; i < last; i++) {
					k2 = scalingKeyFrames[i];
					k3 = scalingKeyFrames[i + 1];
					// Check if the middle keyframe can be calculated by information, if so dont add it
					if (scale && !isLerp(k1.value, k1.time, k2.value, k2.time, k3.value, k3.time, 3)) {
						anim->scaling.push_back(k2);
						k1 = k2;
					}
				}
				if (last > 0)
					anim->scaling.push_back(scalingKeyFrames[last]);
			}
		}
	};

} }

#endif //FBXCONV_READERS_FBXANIMATION_H