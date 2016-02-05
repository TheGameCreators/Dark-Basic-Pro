#ifndef DBPROPHYSICSMASTER_RIGID_BODY_H
#define DBPROPHYSICSMASTER_RIGID_BODY_H


enum eRefFrame{LOCAL,PARENT,WORLD,COFG};

class RigidBody
{
friend class Vehicle;
	public:
		RigidBody();
		~RigidBody();

		void addForce(const Vector3 * force, eRefFrame fref, const Vector3 * point,eRefFrame pref);
        void addTorq(const Vector3 * torq, eRefFrame tref);
		void addImpulse(const Vector3 * impulse, eRefFrame iref, const Vector3 * point,eRefFrame pref);
        void addRotImpulse(const Vector3 * impulse, eRefFrame iref, const Vector3 * point,eRefFrame pref);
		void addRotImpulse(const Vector3 * rotImp, eRefFrame iref);

		void addImpulseAndUpdate(const Vector3 * impulse, eRefFrame iref, const Vector3 * point,eRefFrame pref);


		void getPointWorldVel(const Vector3& localP, Vector3 * velOut);		

		void Reset(bool resetTransforms);

        void clearForces(){totForce.Init();totTorq.Init();totImpulse.Init();totRotImpulse.Init();}

        //Setting Routines

		void setUpdatedParentMatrix(bool s){updatedParentMatrix=s;}

        void setMass(float m)					{mass=m;if(m!=0)invMass=1.0f/m;else invMass=0.0f;}
        void setCofG(const Vector3 * v)			{CofG=*v;}
        void setMofI(const Matrix * m)			{MofI=*m;invMofI=*m;invMofI.Inverse3x3This();}
        void setVel(const Vector3 * v)			{vel = *v;}
		void setVel(float x, float y, float z)	{vel.Set(x,y,z);}
		void setTotForce(const Vector3 * force)	{totForce=*force;}
        void setTotTorq(const Vector3 * torq)	{totTorq=*torq;}
        void setTotImpulse(const Vector3 * imp)	{totImpulse=*imp;}
        void setTotRotImpulse(const Vector3 * imp) {totRotImpulse=*imp;}

        //ALWAYS use these functions for modifying matrices
		Matrix& Local_RW(); //Invalidates matrices and quaternion
        Quat& quatLocal_RW(); //Invalidates matrices

        //Reading Routines

		const Matrix& World();
		const Matrix& invWorld();
		const Matrix& invLocal();
		const Matrix& Local_RO();	//Use for read only access
		const Quat& quatLocal_RO();	//Use for read only access

        float 			getMass()		{return mass;}
		float		 	getInvMass()	{return invMass;}
        const Vector3& 	getCofG()		{return CofG;}
        const Vector3& 	getPCofG();
        const Matrix& 	getMofI()		{return MofI;}
        const Matrix& 	getInvMofI()	{return invMofI;}
		const Matrix& 	getInvMofIT()	{return invMofIT;}
        const Vector3& 	getVel()		{return vel;}
        const Quat& 	getRotVel(){return qLocalVel;}
        const Vector3& 	getAngVel(){return angVel;}
		const Vector3& 	getAngMomentum(){return angMom;}

        const Vector3&	getTotForce()	{return totForce;}
        const Vector3&	getTotTorq()	{return totTorq;}
        const Vector3&	getTotImpulse()	{return totImpulse;}
        const Vector3&	getTotRotImpulse()	{return totRotImpulse;}

	private:

		//Main physical parameters
		float	mass;
		float	invMass;

		Vector3	CofG;
		Vector3	pCofG; //CofG in parent space
        bool pCofGValid; //We only want to calculate pCofG once per frame


		Matrix	MofI;
		Matrix	invMofI;

		//Dynamics State Information
		Matrix	world;
		Matrix	invworld;
		Matrix	local;
		Matrix	invlocal;

		Quat	qLocal;		//Current orientation quaternion
		Quat	qLocalVel; 	//Mainly used for integration

		Matrix	invMofIT; 	//Inverse inertia tensor at time 't', and at an orientation 'rot'

		Vector3	vel;

		Vector3	angVel;
		Vector3 angMom;

		Vector3	totForce;
		Vector3	totTorq;

		Vector3	totImpulse;
		Vector3	totRotImpulse;

		Vector3 angThresh;

		//Matrix and Quaternion State Information
		bool updatedParentMatrix;
		bool matValid_World;
		bool matValid_invWorld;
		bool matValid_invLocal;
   		bool matValid_Local;
   		bool quatValid_Local;
};


#endif
