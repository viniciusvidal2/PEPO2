//  fob.cc
//
//  Author:
//       Ahmad Dajani <eng.adajani@gmail.com>
//
//  Copyright (c) 2020 Ahmad Dajani
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <cmath> //pow
#include "benchmark/fob.hpp"
#include "utils.hpp"
#include <iostream>
#include <string>
#include <math.h>
#include <sys/stat.h>
#include <ostream>
#include <chrono>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/io/ply_io.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl/filters/extract_indices.h>
#include <pcl/filters/conditional_removal.h>
#include <pcl/filters/statistical_outlier_removal.h>

#include <pcl/kdtree/kdtree_flann.h>
#include <dirent.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/mat.hpp>
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"
#include <opencv2/stitching/detail/blenders.hpp>

#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <Eigen/Core>

Benchmark* fob::Create() {
	return new fob();
}

fob::fob() : Benchmark() {
	boundaries_m = new Boundaries[variablesCount_m];
	//fx1, fy1, cx1, cy1, fx2, fy2, cx2, cy2, roll1, pitch1, yaw1, roll2, pitch2, yaw2
	std::vector<std::vector<int> > pose = Utils::FindPoseRaw();
	std::vector<double> pitch, yaw;
	for (int j = 0; j < pose.size(); j++) {

		pitch.push_back(pose[j][1]);
		yaw.push_back(pose[j][2]);

	}
	double cx, cy, fx, fy;
	fx = 1427.099976; fy = 1449.400024; cx = 960; cy = 540;
	std::vector<double> lb{ yaw[1] - 10, pitch[1] - 10,fx - 100,fy - 100,cx - 20,cy - 20, yaw[6] - 10, pitch[6] - 10,fx-100,fy-100,cx-20,cy-20 };
	std::vector<double> up{ yaw[1] + 10, pitch[1] + 10,fx - 100,fy - 100,cx - 20,cy - 20, yaw[6] + 10, pitch[6] + 10,fx + 100,fy + 100,cx + 20,cy + 20 };

	for (register unsigned int variable = 0; variable < variablesCount_m; variable++) {
		boundaries_m[variable].lowerBound = lb[variable], boundaries_m[variable].upperBound = up[variable];
	}

	Benchmark::setName("fob");
	Benchmark::setDimension(variablesCount_m);
	Benchmark::setBoundaries(boundaries_m);
}

fob::~fob() {
	delete boundaries_m;
}

//double fob::fitness(double x[], std::vector<std::vector<std::vector<cv::KeyPoint>>> bestKey, std::vector<std::string> imagens_src, cv::Mat im360) {
//	
//
//	int m = kpts1.size();
//	double erro = 0;
//	auto start_time = std::chrono::high_resolution_clock::now();
//#pragma omp parallel for 
//for (int j = 0; j < m; j++) {
//	//Keypoints
//	cv::Point2d  kp1 = kpts1[j];
//	cv::Point2d  kp2 = kpts2[j];
//
//	// Pose da CAMERA 1, so existe aqui rotacao, vamos suprimir as translacoes 
//	// pois serao irrelevantes e serao compensadas por outros dados
//	
//
//	// Vamos criar o frustrum da CAMERA 1, assim como nas nossas funcoes, como o desenho do github
//	// Supondo a variavel F e o raio da esfera como F = R = 1, nao interferiu nas experiencias
//	
//	double dx1 = x[2] - double(image1.cols) / 2, dy1 = x[3] - double(image1.rows) / 2;
//
//	double maxX = (float(image1.cols) - 2 * dx1) / (2.0 * x[0]);
//	double minX = (float(image1.cols) + 2 * dx1) / (2.0 * x[0]);
//	double maxY = (float(image1.rows) - 2 * dy1) / (2.0 * x[1]);
//	double minY = (float(image1.rows) + 2 * dy1) / (2.0 * x[1]);
//
//	Eigen::Vector3d p, p1, p2, p3, p4, p5, pCenter;
//	float F = 1;
//	p2 << minX, minY, F;
//	p4 << maxX, maxY, F;
//	p5 << minX, maxY, F;
//	
//	float step_deg = 0.1;
//	// Ponto no frustrum 3D correspondente a feature na imagem 1 em 2D
//	Eigen::Vector3d ponto3d = p5 + (p4 - p5) * kp1.x / image1.cols + (p2 - p5) * kp1.y / image1.rows;
//	// Latitude e longitude no 360
//	double lat = 180 / 3.1415 * (acos(ponto3d[1] / ponto3d.norm())), lon = -180 / 3.1415 * (atan2(ponto3d[2], ponto3d[0]));
//	lon = (lon < 0) ? lon += 360.0 : lon;
//	lat = lat - DEG2RAD(Utils::raw2deg(x[8], "pan"));
//	lon = lon + DEG2RAD(Utils::raw2deg(x[9], "tilt"));
//	
//	int u = int(lon / step_deg), v = im360.rows - 1 - int(lat / step_deg);
//	u = (u >= im360.cols) ? im360.cols - 1 : u; // Nao deixar passar do limite de colunas por seguranca
//	u = (u < 0) ? 0 : u;
//	v = (v >= im360.rows) ? im360.rows - 1 : v; // Nao deixar passar do limite de linhas por seguranca
//	v = (v < 0) ? 0 : v;
//	// Ponto na imagem 360 devido a camera 1, finalmente apos as contas, armazenar
//	Eigen::Vector2d ponto_fc1{ u, v };
//	ponto_fc1.normalize();
//	//pp1[j] = ponto_fc1;
//	// ------------------------------------------------------------------------------------------------
//
//	// Pose da CAMERA 2, so existe aqui rotacao, vamos suprimir as translacoes 
//	// pois serao irrelevantes e serao compensadas por outros dados
//	
//	double dx2 = x[6] - double(image2.cols) / 2, dy2 = x[7] - double(image2.rows) / 2;
//
//	maxX = (float(image2.cols) - 2 * dx2) / (2.0 * x[4]);
//	minX = (float(image2.cols) + 2 * dx2) / (2.0 * x[4]);
//	maxY = (float(image2.rows) - 2 * dy2) / (2.0 * x[5]);
//	minY = (float(image2.rows) + 2 * dy2) / (2.0 * x[5]);
//
//	
//	p2 << minX, minY, F;
//	p4 << maxX, maxY, F;
//	p5 << minX, maxY, F;
//	// Nao usado a principio, pode omitir
//
//	// Ponto no frustrum 3D correspondente a feature na imagem 2 em 2D
//	ponto3d = p5 + (p4 - p5) * kp2.x / image2.cols + (p2 - p5) * kp2.y / image2.rows;
//	// Latitude e longitude no 360
//	lat = 180 / 3.1415 * (acos(ponto3d[1] / ponto3d.norm())); lon = -180 / 3.1415 * (atan2(ponto3d[2], ponto3d[0]));
//	lon = (lon < 0) ? lon += 360.0 : lon;
//	lat = lat - DEG2RAD(Utils::raw2deg(x[10], "pan"));
//	lon = lon + DEG2RAD(Utils::raw2deg(x[11], "tilt"));
//	u = int(lon / step_deg); v = im360.rows - 1 - int(lat / step_deg);
//	u = (u >= im360.cols) ? im360.cols - 1 : u; // Nao deixar passar do limite de colunas por seguranca
//	u = (u < 0) ? 0 : u;
//	v = (v >= im360.rows) ? im360.rows - 1 : v; // Nao deixar passar do limite de linhas por seguranca
//	v = (v < 0) ? 0 : v;
//	// Ponto na imagem 360 devido a camera 2, finalmente apos as contas, armazenar
//	Eigen::Vector2d ponto_fc2{ u, v };
//	 ponto_fc2.normalize();
//	// pp2[j] = ponto_fc2;
//	/// RESULTADO FINAL, para ir formando a FOB, com o somatorio do erro entre os pontos
//	///
//
//	//Eigen::Vector2d p1_norm = ponto_fc2.normalize();  //normalize(ponto_fc1, ponto_fc1.minCoeff() , ponto_fc1.maxCoeff());
//	//Eigen::Vector2d p2_norm = normalize(ponto_fc2, ponto_fc2.minCoeff(), ponto_fc2.maxCoeff());
//	/* erro = erro + (pp1[j] - pp2[j]).norm();*/
//	 erro = erro + (ponto_fc1 - ponto_fc2).norm();
//
//  }//end for
////pp1.normalize();
////pp2.normalize();
////double erro = (pp1 - pp2).norm();
//auto finish_time = std::chrono::high_resolution_clock::now();
//auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count() * 1e-9;
////std::cout <<"fob "<< time<<"\n";
//return erro;
//}
double fob::fitness(double x[], std::vector<std::vector<std::vector<cv::KeyPoint>>> bestKey, std::vector<std::string> imagens_src, cv::Mat im360, cv::Mat image1, cv::Mat image2, std::vector<std::vector<int>> indices) {
	int currentCamera = 0;
	auto start_time = std::chrono::high_resolution_clock::now();

	//double erro = 0;
	std::vector<double> erro;
	erro.resize(bestKey.size());
	
	for (size_t frame0 = currentCamera; frame0 < bestKey.size(); frame0++)
	{
		erro[frame0] = 0;
		
		int l = 0;
		
			for (int j = 0; j < bestKey[frame0].size(); j++)
			{
				std::vector<cv::KeyPoint> kpts1 = bestKey[0][j];
				std::vector<cv::KeyPoint> kpts2 = bestKey[0][j + 1];
				int frame1 = indices[frame0][l];
				if (bestKey[frame0][j].size() > 20) {
				int m = kpts1.size();
#pragma omp parallel for 
				for (int k = 0; k < m; k++) {

					cv::KeyPoint kp1 = kpts1[k];
					cv::KeyPoint kp2 = kpts2[k];

					double dx1 = x[frame0*6+4] - double(image1.cols) / 2, dy1 = x[frame0*6+5] - double(image1.rows) / 2;

					double maxX = (float(image1.cols) - 2 * dx1) / (2.0 * x[frame0*6 + 2]);
					double minX = (float(image1.cols) + 2 * dx1) / (2.0 * x[frame0 *6 + 2]);
					double maxY = (float(image1.rows) - 2 * dy1) / (2.0 * x[frame0*6 + 3]);
					double minY = (float(image1.rows) + 2 * dy1) / (2.0 * x[frame0*6 + 3]);

					Eigen::Vector3d p, p1, p2, p3, p4, p5, pCenter;
					float F = 1;
					p2 << minX, minY, F;
					p4 << maxX, maxY, F;
					p5 << minX, maxY, F;

					float step_deg = 0.1;
					// Ponto no frustrum 3D correspondente a feature na imagem 1 em 2D
					Eigen::Vector3d ponto3d = p5 + (p4 - p5) * kp1.pt.x / image1.cols + (p2 - p5) * kp1.pt.y / image1.rows;
					// Latitude e longitude no 360
					double lat = 180 / 3.1415 * (acos(ponto3d[1] / ponto3d.norm())), lon = -180 / 3.1415 * (atan2(ponto3d[2], ponto3d[0]));
					lon = (lon < 0) ? lon += 360.0 : lon;
					lat = lat - DEG2RAD(Utils::raw2deg(x[frame0*6+1], "pan"));
					lon = lon + DEG2RAD(Utils::raw2deg(x[frame0*6], "tilt"));

					int u = int(lon / step_deg), v = im360.rows - 1 - int(lat / step_deg);
					u = (u >= im360.cols) ? im360.cols - 1 : u; // Nao deixar passar do limite de colunas por seguranca
					u = (u < 0) ? 0 : u;
					v = (v >= im360.rows) ? im360.rows - 1 : v; // Nao deixar passar do limite de linhas por seguranca
					v = (v < 0) ? 0 : v;
					// Ponto na imagem 360 devido a camera 1, finalmente apos as contas, armazenar
					Eigen::Vector2d ponto_fc1{ u, v };
					ponto_fc1.normalize();
					//pp1[j] = ponto_fc1;
					// ------------------------------------------------------------------------------------------------

					// Pose da CAMERA 2, so existe aqui rotacao, vamos suprimir as translacoes 
					// pois serao irrelevantes e serao compensadas por outros dados

					double dx2 = x[frame1*6+4] - double(image2.cols) / 2, dy2 = x[frame1*6+5] - double(image2.rows) / 2;

					maxX = (float(image2.cols) - 2 * dx2) / (2.0 * x[frame1*6+2]);
					minX = (float(image2.cols) + 2 * dx2) / (2.0 * x[frame1 * 6 + 2]);
					maxY = (float(image2.rows) - 2 * dy2) / (2.0 * x[frame1 * 6 + 3]);
					minY = (float(image2.rows) + 2 * dy2) / (2.0 * x[frame1 * 6 + 3]);


					p2 << minX, minY, F;
					p4 << maxX, maxY, F;
					p5 << minX, maxY, F;
					// Nao usado a principio, pode omitir

					// Ponto no frustrum 3D correspondente a feature na imagem 2 em 2D
					ponto3d = p5 + (p4 - p5) * kp2.pt.x / image2.cols + (p2 - p5) * kp2.pt.y / image2.rows;
					// Latitude e longitude no 360
					lat = 180 / 3.1415 * (acos(ponto3d[1] / ponto3d.norm())); lon = -180 / 3.1415 * (atan2(ponto3d[2], ponto3d[0]));
					lon = (lon < 0) ? lon += 360.0 : lon;
					lat = lat - DEG2RAD(Utils::raw2deg(x[frame1*6+1], "pan"));
					lon = lon + DEG2RAD(Utils::raw2deg(x[frame1*6], "tilt"));
					u = int(lon / step_deg); v = im360.rows - 1 - int(lat / step_deg);
					u = (u >= im360.cols) ? im360.cols - 1 : u; // Nao deixar passar do limite de colunas por seguranca
					u = (u < 0) ? 0 : u;
					v = (v >= im360.rows) ? im360.rows - 1 : v; // Nao deixar passar do limite de linhas por seguranca
					v = (v < 0) ? 0 : v;
					// Ponto na imagem 360 devido a camera 2, finalmente apos as contas, armazenar
					Eigen::Vector2d ponto_fc2{ u, v };
					ponto_fc2.normalize();
					// pp2[j] = ponto_fc2;
					/// RESULTADO FINAL, para ir formando a FOB, com o somatorio do erro entre os pontos
					///

					//Eigen::Vector2d p1_norm = ponto_fc2.normalize();  //normalize(ponto_fc1, ponto_fc1.minCoeff() , ponto_fc1.maxCoeff());
					//Eigen::Vector2d p2_norm = normalize(ponto_fc2, ponto_fc2.minCoeff(), ponto_fc2.maxCoeff());
					/* erro = erro + (pp1[j] - pp2[j]).norm();*/
					erro[frame0] = erro[frame0] + (ponto_fc1 - ponto_fc2).norm();

					

				}
				l++;
				j++;
				
			}
		}
		
		

	}
	double erroT;
	//media dos erros de cada camera
	erroT = std::accumulate(erro.begin(), erro.end(), 0.0)/erro.size();
	auto finish_time = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count() * 1e-9;
	//std::cout <<"fob "<< time<<"\n";
	return erroT;
}