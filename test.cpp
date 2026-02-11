#define DBG_MACRO_NO_WARNING
#include "dbg.h"

#include <vector>
#include <iostream>

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <Eigen/Geometry>

using RowVectors = Eigen::Matrix<double, Eigen::Dynamic, 3, Eigen::RowMajor>;
using RowVectorsNx3 = RowVectors;
using RowVectorsNx2 = Eigen::Matrix<double, Eigen::Dynamic, 2, Eigen::RowMajor>;

// Convert quaternion (w, x, y, z) to rotation matrix
inline Eigen::Matrix3d qwxyz2mat(double w, double x, double y, double z) {
	Eigen::Matrix3d mat;
	double w2 = w * w, x2 = x * x, y2 = y * y, z2 = z * z;
	double xy = x * y, xz = x * z, yz = y * z, wx = w * x, wy = w * y, wz = w * z;

	mat(0, 0) = 1 - 2 * (y2 + z2);
	mat(0, 1) = 2 * (xy - wz);
	mat(0, 2) = 2 * (xz + wy);

	mat(1, 0) = 2 * (xy + wz);
	mat(1, 1) = 1 - 2 * (x2 + z2);
	mat(1, 2) = 2 * (yz - wx);

	mat(2, 0) = 2 * (xz - wy);
	mat(2, 1) = 2 * (yz + wx);
	mat(2, 2) = 1 - 2 * (x2 + y2);

	return mat;
}

void update(Eigen::Ref<RowVectorsNx2> xys) {
	xys.col(0) *= 10;
	xys.col(1) *= 20;
}

int main(int argc, char **argv) {
	dbg(EIGEN_WORLD_VERSION);
	dbg(EIGEN_MAJOR_VERSION);
	dbg(EIGEN_MINOR_VERSION);

	Eigen::Vector3d xyz(1, 2, 3);
	dbg(xyz.transpose());

	Eigen::VectorXd coeffs = (Eigen::VectorXd(5) << 4, 5, 6, 7, 8).finished();
	dbg(coeffs.transpose());

	auto abcd = std::vector<double>{1, 3, 5, 7};
	dbg(abcd);
	Eigen::Map<Eigen::Vector4d>(&abcd[0], 4) *= 10;
	dbg(abcd);

	auto mat = std::vector<double>{0, 1, 2, 3, 4, 5};
	Eigen::Map<RowVectorsNx3> M(&mat[0], 2, 3);
	std::cout << M << std::endl;
	update(M.leftCols(2));
	std::cout << M << std::endl;
	// dbg(M); // https://github.com/sharkdp/dbg-macro/issues/131

	// https://eigen.tuxfamily.org/dox/group__SparseCore__Module.html

	// Test Eigen/Geometry - Quaternion construction and conversion to Matrix
	std::cout << "\n=== Testing Eigen/Geometry ===" << std::endl;

	// Create a quaternion from axis-angle
	Eigen::Quaterniond q1(Eigen::AngleAxisd(M_PI / 4, Eigen::Vector3d::UnitZ()));
	std::cout << "Quaternion (45° around Z-axis):\n" << q1.coeffs().transpose() << std::endl;

	// Convert quaternion to rotation matrix
	Eigen::Matrix3d rotMat = q1.toRotationMatrix();
	std::cout << "Rotation Matrix:\n" << rotMat << std::endl;

	// Create quaternion from Euler angles
	Eigen::Quaterniond q2 = Eigen::AngleAxisd(0.1, Eigen::Vector3d::UnitX())
	                      * Eigen::AngleAxisd(0.2, Eigen::Vector3d::UnitY())
	                      * Eigen::AngleAxisd(0.3, Eigen::Vector3d::UnitZ());
	dbg(q2.coeffs().transpose());

	// Quaternion multiplication and normalization
	Eigen::Quaterniond q3 = q1 * q2;
	q3.normalize();
	dbg(q3.coeffs().transpose());

	// Compare Eigen's toRotationMatrix() with qwxyz2mat()
	std::cout << "\n=== Compare qwxyz2mat vs Eigen ===" << std::endl;

	// Test with q1, q2, q3
	Eigen::Quaterniond quats[] = {q1, q2, q3};
	const char* names[] = {"q1", "q2", "q3"};
	for (int i = 0; i < 3; i++) {
		double w = quats[i].w(), x = quats[i].x(), y = quats[i].y(), z = quats[i].z();
		Eigen::Matrix3d eigen_mat = quats[i].toRotationMatrix();
		Eigen::Matrix3d my_mat = qwxyz2mat(w, x, y, z);
		double err = (eigen_mat - my_mat).norm();
		std::cout << names[i] << ": max error = " << err << (err < 1e-14 ? " [PASS]" : " [FAIL]") << std::endl;
	}

	return 0;
}
