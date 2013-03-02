#include  <sys/types.h>
#include <sys/stat.h>
#include "Subsystem.h"
#include "WPIObjMgr.h"
#include "Config.h"

#include "WPILib.h"
#include "Vision/RGBImage.h"
#include "Vision/BinaryImage.h"
#include "Math.h"
#include "Config.h"
#include <iostream>
#include <fcntl.h>

/**
 * Sample program to use NIVision to find rectangles in the scene that are illuminated
 * by a LED ring light (similar to the model from FIRSTChoice). The camera sensitivity
 * is set very low so as to only show light sources and remove any distracting parts
 * of the image.
 * 
 * The CriteriaCollection is the set of criteria that is used to filter the set of
 * rectangles that are detected. In this example we're looking for rectangles with
 * a minimum width of 30 pixels and maximum of 400 pixels.
 * 
 * The algorithm first does a color threshold operation that only takes objects in the
 * scene that have a bright green color component. Then a convex hull operation fills 
 * all the rectangle outlines (even the partially occluded ones). Then a small object filter
 * removes small particles that might be caused by green reflection scattered from other 
 * parts of the scene. Finally all particles are scored on rectangularity, aspect ratio,
 * and hollowness to determine if they match the target.
 *
 * Look in the VisionImages directory inside the project that is created for the sample
 * images as well as the NI Vision Assistant file that contains the vision command
 * chain (open it with the Vision Assistant)
 */

//Camera constants used for distance calculation
#define X_IMAGE_RES 320		//X Image resolution in pixels, should be 160, 320 or 640
//#define VIEW_ANGLE 38		//Axis 206 camera
#define VIEW_ANGLE 47.5f  //Axis M1011 camera
#define PI 3.141592653

//Score limits used for target identification
#define RECTANGULARITY_LIMIT 60
#define ASPECT_RATIO_LIMIT 75
#define X_EDGE_LIMIT 40
#define Y_EDGE_LIMIT 60

//Minimum area of particles to be considered
#define AREA_MINIMUM 500

//Edge profile constants used for hollowness score calculation
#define XMAXSIZE 24
#define XMINSIZE 24
#define YMAXSIZE 24
#define YMINSIZE 48

class TargetV2 : public Spyder::Subsystem {
	STATUS pipeDrv();
	STATUS pipeDevCreate(const char*, int, int);
	//Structure to represent the scores for the various tests used for target identification
	struct Scores {
		double rectangularity;
		double aspectRatioInner;
		double aspectRatioOuter;
		double xEdge;
		double yEdge;

	};

	struct Control {
		double xAdj;
		double yAdj;
		float dist;
	};

	Spyder::TwoIntConfig Hue;
	Spyder::TwoIntConfig Sat;
	Spyder::TwoIntConfig Val;
	
	Scores *scores;
	
	Control cont_side;
	Control cont_mid;
	
	int len, MidOutFd, SideOutFd;
	
	void ProcessLoop();
	int min(double& val, int bound) // return the smaller of the inputs
	{
		if (val < bound) {
			return bound;
		} else {
			return (int) val;
		}
	}
	int max(double& val, int bound) {
		if (val > bound) {
			return bound;
		} else {
			return (int) val;
		}
	}
	int max(int val, int bound) {
		if (val > bound) {
			return bound;
		} else {
			return (int) val;
		}
	}
public:
	TargetV2() :
		Spyder::Subsystem("Target"), Hue("hue_thresh", 0, 10), Sat("sat_thresh", 50, 150),
				Val("val_thresh", 50, 150) 
	{
		
		cont_side.xAdj = 0.0f;
		cont_side.yAdj = 0.0f;
		cont_mid.xAdj = 0.0f;
		cont_mid.yAdj = 0.0f;
		pipeDrv();
		pipeDevCreate("/pipes/MidTargetOut", 30, sizeof(Control));
		pipeDevCreate("/pipes/SideTargetOut", 30, sizeof(Control));
		MidOutFd = open("/pipes/MidTargetOut", O_RDWR, 0664);
		SideOutFd = open("/pipes/SideTargetOut", O_RDWR, 0664);
	}

	double computeDistance(BinaryImage *image, ParticleAnalysisReport *report,
			bool outer);
	double scoreAspectRatio(BinaryImage *image, ParticleAnalysisReport *report,
			bool outer);
	bool scoreCompare(Scores scores, bool outer);
	double scoreRectangularity(ParticleAnalysisReport *report);
	
	Control GetCenter()
	{
		return cont_mid;
	}
	Control GetSide()
	{
		return cont_side;
	}
	void Init(Spyder::RunModes rmode){}
	void Periodic(Spyder::RunModes rmode)
	{
		write(MidOutFd, (char*)&cont_mid, sizeof(struct Control));
		write(SideOutFd, (char*)&cont_side, sizeof(struct Control));
	}
	void RobotInit()
	{
		
	}
	
	
};
TargetV2 target;

void TargetV2::ProcessLoop() {

	Threshold threshold(Hue.GetVar(1), Hue.GetVar(2), Sat.GetVar(1),
			Sat.GetVar(2), Val.GetVar(1), Val.GetVar(2)); //HSV threshold criteria, ranges are in that order ie. Hue is 60-100
	ParticleFilterCriteria2 criteria[] = { { IMAQ_MT_AREA, AREA_MINIMUM, 65535,
			false, false } }; //Particle filter criteria, used to filter out small particles
	//AxisCamera &camera = AxisCamera::GetInstance("10.16.22.11");	//To use the Axis camera uncomment this line

	AxisCamera &camera = AxisCamera::GetInstance("10.16.22.11");
	sleep(3);
	while (1) {
		sleep(3);
		/**
		 * Do the image capture with the camera and apply the algorithm described above. This
		 * sample will either get images from the camera or from an image file stored in the top
		 * level directory in the flash memory on the cRIO. The file name in this case is "testImage.jpg"
		 */
		ColorImage *image = NULL;
		image = new RGBImage("/testImage.jpg"); // get the sample image from the cRIO flash

		camera.GetImage(image); //To get the images from the camera comment the line above and uncomment this one
		if (image == NULL)
			printf("image is null!\n");
		if ((image == (void*) 0) || (image->GetWidth() == 0)
				|| (image->GetHeight() == 0) || (image == NULL)) {
			printf("invalid image!\n");
		} else {
			//BinaryImage *thresholdImage = image->ThresholdHSV(threshold);	// get just the green target pixels
			//thresholdImage->Write("/threshold.bmp");
			BinaryImage *thresholdImage2 = image->ThresholdRGB(20, 255, 0, 25,
					0, 25);
			thresholdImage2->Write("/threshold2.bmp");
			BinaryImage *convexHullImage = thresholdImage2->ConvexHull(false); // fill in partial and full rectangles
			convexHullImage->Write("/ConvexHull.bmp");
			BinaryImage *filteredImage = convexHullImage->ParticleFilter(
					criteria, 1); //Remove small particles
			filteredImage->Write("Filtered.bmp");

			vector<ParticleAnalysisReport> *reports =
					filteredImage->GetOrderedParticleAnalysisReports(); //get a particle analysis report for each particle
			scores = new Scores[reports->size()];

			//Iterate through each particle, scoring it and determining whether it is a target or not
			for (unsigned i = 0; i < reports->size(); i++) {
				ParticleAnalysisReport *report = &(reports->at(i));

				scores[i].rectangularity = scoreRectangularity(report);
				scores[i].aspectRatioOuter = scoreAspectRatio(filteredImage,
						report, true);
				scores[i].aspectRatioInner = scoreAspectRatio(filteredImage,
						report, false);

				if (scoreCompare(scores[i], false)) {
					printf(
							"particle: %d  is a High Goal  centerX: %f  centerY: %f \n",
							i, report->center_mass_x_normalized,
							report->center_mass_y_normalized);
					cont_mid.xAdj = report->center_mass_x_normalized; //update our center goal struct
					cont_mid.yAdj = report->center_mass_y_normalized;
					cont_mid.dist = computeDistance(thresholdImage2, report, false);
					printf("Distance: %f \n", cont_mid.dist);
				} else if (scoreCompare(scores[i], true)) {
					printf(
							"particle: %d  is a Middle Goal  centerX: %f  centerY: %f \n",
							i, report->center_mass_x_normalized,
							report->center_mass_y_normalized);
					cont_side.xAdj = report->center_mass_x_normalized; //update our side goal struct
					cont_side.yAdj = report->center_mass_y_normalized;
					cont_side.dist = computeDistance(thresholdImage2, report, true);
					printf("Distance: %f \n", cont_side.dist);
				} else {
					printf(
							"particle: %d  is not a goal  centerX: %f  centerY: %f \n",
							i, report->center_mass_x_normalized,
							report->center_mass_y_normalized);
				}
				printf("rect: %f  ARinner: %f \n", scores[i].rectangularity,
						scores[i].aspectRatioInner);
				printf("ARouter: %f\n", scores[i].aspectRatioOuter);
			}
			printf("FOV %f", VIEW_ANGLE);
			printf("\n");

			// be sure to delete images after using them
			delete filteredImage;
			delete convexHullImage;
			delete thresholdImage2;
			delete image;

			//delete allocated reports and Scores objects also
			delete scores;
			delete reports;
		}
	}
}
/**
 * Computes the estimated distance to a target using the height of the particle in the image. For more information and graphics
 * showing the math behind this approach see the Vision Processing section of the ScreenStepsLive documentation.
 * 
 * @param image The image to use for measuring the particle estimated rectangle
 * @param report The Particle Analysis Report for the particle
 * @param outer True if the particle should be treated as an outer target, false to treat it as a center target
 * @return The estimated distance to the target in Feet.
 */
double TargetV2::computeDistance(BinaryImage *image,
		ParticleAnalysisReport *report, bool outer) {
	double rectShort, height;
	int targetHeight;

	//imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE, &rectShort);
	//using the smaller of the estimated rectangle short side and the bounding rectangle height results in better performance
	//on skewed rectangles
	//height = min(report->boundingRect.height, rectShort);
	height = report->boundingRect.height;
	//targetHeight = outer ? 16 : 16;
	targetHeight = outer ? 32 : 32;
	std::cout << "\n height: " << height << "\nTarget Height: " << targetHeight
			<< "\nbounding: " << report->boundingRect.height << "\nimaq: "
			<< rectShort << std::endl;

	//figure out equation
	//return (height/X_IMAGE_RES) * ( (targetHeight/2) / (atan (VIEW_ANGLE*PI/(2*180) ) ) );
	//0.11							8				0.389
	float dist = ((((targetHeight * 240) / height) / 2) / tan(
			((VIEW_ANGLE * PI) / 180.0) / 2.0) / 12);//240 is the camera Y res
	std::cout << "DISTANCE: " << dist << std::endl;
	return dist;
}

/**
 * Compares scores to defined limits and returns true if the particle appears to be a target
 * 
 * @param scores The structure containing the scores to compare
 * @param outer True if the particle should be treated as an outer target, false to treat it as a center target
 * 
 * @return True if the particle meets all limits, false otherwise
 */
bool TargetV2::scoreCompare(Scores scores, bool outer) {
	bool isTarget = true;

	isTarget &= scores.rectangularity > RECTANGULARITY_LIMIT;
	isTarget ? printf("still a target: rect test.\n") : printf(
			"failed rect test\n");
	if (outer) {
		isTarget &= scores.aspectRatioOuter > ASPECT_RATIO_LIMIT;
	} else {
		isTarget &= scores.aspectRatioInner > ASPECT_RATIO_LIMIT;
	}
	isTarget ? printf("still a target: aspect ratio\n") : printf(
			"aspect ratio failed\n");
	//isTarget &= scores.xEdge > X_EDGE_LIMIT;
	//isTarget? printf("x edge: pass\n"):printf("xedge:fail\n");
	//isTarget &= scores.yEdge > Y_EDGE_LIMIT;
	//isTarget? printf("y edge: pass\n"):printf("yedge:fail\n");

	return isTarget;
}
/**
 * Computes a score (0-100) comparing the aspect ratio to the ideal aspect ratio for the target. This method uses
 * the equivalent rectangle sides to determine aspect ratio as it performs better as the target gets skewed by moving
 * to the left or right. The equivalent rectangle is the rectangle with sides x and y where particle area= x*y
 * and particle perimeter= 2x+2y
 * 
 * @param image The image containing the particle to score, needed to perform additional measurements
 * @param report The Particle Analysis Report for the particle, used for the width, height, and particle number
 * @param outer	Indicates whether the particle aspect ratio should be compared to the ratio for the inner target or the outer
 * @return The aspect ratio score (0-100)
 */
double TargetV2::scoreAspectRatio(BinaryImage *image,
		ParticleAnalysisReport *report, bool outer) {
	double rectLong, rectShort, idealAspectRatio, aspectRatio;
	idealAspectRatio = outer ? (62 / 29) : (62 / 20); //Dimensions of goal opening + 4 inches on all 4 sides for reflective tape

	imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0,
			IMAQ_MT_EQUIVALENT_RECT_LONG_SIDE, &rectLong);
	imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0,
			IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE, &rectShort);

	//Divide width by height to measure aspect ratio
	if (report->boundingRect.width > report->boundingRect.height) {
		//particle is wider than it is tall, divide long by short
		aspectRatio = 100 * (1 - fabs(
				(1 - ((rectLong / rectShort) / idealAspectRatio))));
	} else {
		//particle is taller than it is wide, divide short by long
		aspectRatio = 100 * (1 - fabs(
				(1 - ((rectShort / rectLong) / idealAspectRatio))));
	}
	printf("aspect score: %d\n", max(0, min(aspectRatio, 100)));
	return (max(0, min(aspectRatio, 100))); //force to be in range 0-100
}
/**
 * Computes a score (0-100) estimating how rectangular the particle is by comparing the area of the particle
 * to the area of the bounding box surrounding it. A perfect rectangle would cover the entire bounding box.
 * 
 * @param report The Particle Analysis Report for the particle to score
 * @return The rectangularity score (0-100)
 */
double TargetV2::scoreRectangularity(ParticleAnalysisReport *report) {
	if (report->boundingRect.width * report->boundingRect.height != 0) {
		printf(
				"rect score: %d\n",
				100 * report->particleArea / (report->boundingRect.width
						* report->boundingRect.height));
		return 100 * report->particleArea / (report->boundingRect.width
				* report->boundingRect.height);
	} else {
		return 0;
	}
}
