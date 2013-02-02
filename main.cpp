#include <WPILib.h>


class RobotMain : public IterativeRobot
{
	public:
		virtual void StartCompetition()
		{
		}
		
		virtual void RobotInit()
		{
			this->SetPeriod(0.0);
		}
		
		virtual void DisabledInit()
		{
		}
		
		virtual void AutonomousInit()
		{
		}
		
		virtual void TeleopInit()
		{
		}
		
		virtual void TestInit()
		{
		}
		
		virtual void DisabledPeriodic()
		{
		}
		
		virtual void AutonomousPeriodic()
		{
		}
		
		virtual void TeleopPeriodic()
		{
		}
		
		virtual void TestPeriodic()
		{
		}
		
	protected:
		virtual ~RobotMain()
		{
		}
		
		RobotMain()
		{
		}
};
