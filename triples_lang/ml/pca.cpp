#include <iostream>
#include <vector>
#include <Eigen/Dense> // Install Eigen: https://eigen.tuxfamily.org/

using namespace Eigen;
using namespace std;

// Function to perform PCA
MatrixXd PCA(const MatrixXd &data, int target_dim) {
    if (target_dim <= 0 || target_dim > data.cols()) {
        throw invalid_argument("Invalid target dimension.");
    }

    // Step 1: Center the data (subtract mean of each column)
    MatrixXd centered = data.rowwise() - data.colwise().mean();

    // Step 2: Compute covariance matrix
    MatrixXd cov = (centered.adjoint() * centered) / double(data.rows() - 1);

    // Step 3: Eigen decomposition
    SelfAdjointEigenSolver<MatrixXd> eig(cov);
    if (eig.info() != Success) {
        throw runtime_error("Eigen decomposition failed.");
    }

    // Step 4: Sort eigenvalues in descending order
    VectorXd eigenvalues = eig.eigenvalues().reverse();
    MatrixXd eigenvectors = eig.eigenvectors().rowwise().reverse();

    // Step 5: Select top-k eigenvectors
    MatrixXd projection_matrix = eigenvectors.leftCols(target_dim);

    // Step 6: Project data
    return centered * projection_matrix;
}

int main() {
    try {
        // Example dataset: 5 samples, 3 features
        MatrixXd data(5, 3);
        data << 2.5, 2.4, 1.0,
                0.5, 0.7, 0.8,
                2.2, 2.9, 1.1,
                1.9, 2.2, 0.9,
                3.1, 3.0, 1.2;

        cout << "Original Data:\n" << data << "\n\n";

        // Reduce to 2 dimensions
        MatrixXd reduced = PCA(data, 2);

        cout << "Reduced Data (2D):\n" << reduced << "\n";
    }
    catch (const exception &e) {
        cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
