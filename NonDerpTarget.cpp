#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"
#include "HerpTarget.h"
#include <WPILib.h>

TargetData tgtData;

class HerpTarget : public Spyder::Subsystem
{
	private:
		ReentrantSemaphore m_lock;
		HSLImage *m_image;
		
		void ImageLoop()
		{
			while(true)
			{
				AxisCamera &camera = AxisCamera::GetInstance("10.16.22.11");
				m_lock.take();
				if(m_image)
				{
					delete m_image;
				}
				m_image = camera.GetImage();
				m_lock.give();
			}
		}
		
		Task m_ImageLpTask;
	public:
		static void LaunchImageLoop(HerpTarget *herp)
		{
			herp->ImageLoop();
		}
		
		HerpTarget() : Spyder::Subsystem("HerpTarget"), m_ImageLpTask("HerpTargetImageLoop", (FUNCPTR)LaunchImageLoop)
		{
		}
		
		virtual void Init(Spyder::RunModes runmode)
		{
		}
		
		virtual void Periodic(Spyder::RunModes runmode)
		{
			m_lock.take();
			if(!m_image)
				return;
			ParticleFilterCriteria2 criteria[] = { { IMAQ_MT_AREA, 500, 65535, false, false } }; //Particle filter criteria, used to filter out small particles
			BinaryImage *binImg = m_image->ThresholdRGB(20, 255, 0, 25, 0, 25);
			BinaryImage *convex = binImg->ConvexHull(false);
			BinaryImage *noParticles = convex->ParticleFilter(criteria, 1);
			
			delete binImg;
			m_lock.give();
		}
		
		virtual void RobotInit()
		{
			m_ImageLpTask.Start((UINT32)this);
		}
};
