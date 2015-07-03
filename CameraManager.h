#pragma once
#include <DirectXMath.h>
#include "DirectXMathConverter.h"
#include <Windows.h>

using namespace DirectX;




//////////////////////////////////////////////////////
// Stores View and Projection matrices used by shaders
// to translate 3D world into 2D screen surface
// Camera can be moved and rotated. Also, user can change 
// camera's target and position
////////////////////////////////////////////////////////
class GCamera
{
public:
	// Constructs default camera looking at 0,0,0
	// placed at 0,0,-1 with up vector 0,1,0 (note that mUp is NOT a vector - it's vector's end)
	GCamera(void);
	// Create camera, based on another one
	GCamera(const GCamera& camera);
	// Copy all camera's parameters
	GCamera& operator=(const GCamera& camera);
	~GCamera(void) {}

private:
	// Initialize camera's View matrix from mPosition, mTarget and mUp coordinates
	void lookatViewMatrix();
	void buildViewMatrix();
public:
	// Initialize camera's perspective Projection matrix
	void InitProjMatrix(const float angle, const float client_width, const float client_height,
		const float nearest, const float farthest);
	// Initialize camera's orthogonal projection
	void InitOrthoMatrix(const float client_width, const float client_height,
		const float near_plane, const float far_plane);

	// Resize matrices when window size changes
	void OnResize(uint32_t new_width, uint32_t new_height);

	///////////////////////////////////////////////
	/*** View matrix transformation interfaces ***/
	///////////////////////////////////////////////

	// Move camera
	void Move(XMFLOAT3 direction);
	// Rotate camera around `axis` by `degrees`. Camera's position is a 
	// pivot point of rotation, so it doesn't change
	void Rotate(XMFLOAT3 axis, float degrees);
	// Set camera position coordinates
	void Position(XMFLOAT3& new_position);
	// Get camera position coordinates
	const XMFLOAT3& Position() const { return mPosition; }
	// Change camera target position
	void Target(XMFLOAT3 new_target);
	// Get camera's target position coordinates
	const XMFLOAT3& Target() const { return mTarget; }
	// Get camera's up vector
	const XMFLOAT3 Up() { return GMathVF(GMathFV(mUp) - GMathFV(mPosition)); }
	// Get camera's look at target vector
	 XMFLOAT3 LookAtTarget() { return GMathVF(GMathFV(mTarget) - GMathFV(mPosition)); }
	// Returns transposed camera's View matrix	
	const XMFLOAT4X4 View() { return GMathMF(XMMatrixTranspose(GMathFM(mView))); }

	/////////////////////////////////////////////////////
	/*** Projection matrix transformation interfaces ***/
	/////////////////////////////////////////////////////

	// Set view frustum's angle
	void Angle(float angle);
	// Get view frustum's angle
	const float& Angle() const { return mAngle; }

	// Set nearest culling plane distance from view frustum's projection plane
	void NearestPlane(float nearest);
	// Set farthest culling plane distance from view frustum's projection plane
	void FarthestPlane(float farthest);

	// Returns transposed camera's Projection matrix
	const XMFLOAT4X4 Proj() { return GMathMF(XMMatrixTranspose(GMathFM(mProj))); }
	// Returns transposed orthogonal camera matrix
	const XMFLOAT4X4 Ortho() { return GMathMF(XMMatrixTranspose(GMathFM(mOrtho))); }

	const XMFLOAT4X4 ProjView() {return GMathMF(XMMatrixTranspose(GMathFM(mView)*(GMathFM(mProj)))); }


	const XMFLOAT4X4 InvScreenProjView() {


		XMFLOAT4X4 matScreen = XMFLOAT4X4(2/mClientWidth,0,0,0,0, -2/ mClientHeight,0,0,0,0,1,0,-1,1,0,1);
		return GMathMF(XMMatrixTranspose(GMathFM(matScreen)*XMMatrixInverse(&XMMatrixDeterminant(GMathFM(mProj)), GMathFM(mProj))*
			XMMatrixInverse(&XMMatrixDeterminant(GMathFM(mView)), GMathFM(mView))));
	}

	void KeyDown(UINT key);
	void KeyUp(UINT key);

	void Update();

	void InputPress( float x, float y);

	void InputMove( float x, float y);

	void InputRelease();

	void SetSpeed(float speed) { mSpeed=speed; }

private:
	const float ROTATION_GAIN = 0.004f;
	const float MOVEMENT_GAIN =0.1f;
	float mSpeed;
	float m_pitch, m_yaw;			// orientation euler angles in radians
	/*** Input parameters ***/
	// properties of the Look control
	bool m_lookInUse;			    // specifies whether the look control is in use
	int m_lookPointerID;		    // id of the pointer in this control
	XMFLOAT2 m_lookLastPoint;		    // last point (from last frame)
	XMFLOAT2 m_lookLastDelta;		    // for smoothing
	bool m_forward;
	bool m_back;
	bool m_left;
	bool m_right;
	


	/*** Camera parameters ***/
	XMFLOAT3 mPosition;		// Camera's coordinates
	XMFLOAT3 mTarget;		// View target's coordinates
	XMFLOAT3 mUp;			// Camera's up vector end coordinates
	XMFLOAT3 mLook;
	XMFLOAT3 mForward;
	XMFLOAT3 mRight;
							/*** Projection parameters ***/
	float mAngle;			// Angle of view frustum
	float mClientWidth;		// Window's width
	float mClientHeight;	// Window's height
	float mNearest;			// Nearest view frustum plane
	float mFarthest;		// Farthest view frustum plane

	


	XMFLOAT4X4  mView;		// View matrix
	XMFLOAT4X4	mProj;		// Projection matrix
	XMFLOAT4X4	mOrtho;		// Ortho matrix for drawing without tranformation
};

