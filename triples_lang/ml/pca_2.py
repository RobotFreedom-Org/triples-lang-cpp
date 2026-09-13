#include <iostream>
#include <Eigen/Dense>

class PCA {
public:
    explicit PCA(int num_components) : num_components_(num_components) {}

    // Fits the model and transforms the data to a lower-dimensional space
    Eigen::MatrixXd fit_transform(const Eigen::MatrixXd& X) {
        // 1. Center the data (Subtract the mean of each column)
        mean_ = X.colwise().mean();
        Eigen::MatrixXd centered = X.rowwise() - mean_.transpose();

        // 2. Calculate the Covariance Matrix
        // Formula: (Z^T * Z) / (n - 1)
        double n = static_cast<double>(X.rows());
        Eigen::MatrixXd covariance = (centered.transpose() * centered) / (n - 1.0);

        // 3. Perform Eigen Decomposition
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> solver(covariance);
        
        // Eigenvalues and eigenvectors are sorted in ascending order by SelfAdjointEigenSolver
        Eigen::VectorXd all_eigenvalues = solver.eigenvalues();
        Eigen::MatrixXd all_eigenvectors = solver.eigenvectors();

        // 4. Extract top K eigenvectors (flip columns to sort in descending order)
        // We pick the columns from the end because they contain the largest eigenvalues
        components_ = all_eigenvectors.rightCols(num_components_).rowwise().reverse();

        // 5. Project data into the lower-dimensional space
        return centered * components_;
    }

    // Transform new data using already fitted components
    Eigen::MatrixXd transform(const Eigen::MatrixXd& X) const {
        Eigen::MatrixXd centered = X.rowwise() - mean_.transpose();
        return centered * components_;
    }

private:
    int num_components_;
    Eigen::VectorXd mean_;
    Eigen::MatrixXd components_;
};

int main() {
    // Example Dataset: 5 samples, 3 features (High-Dimensional Space)
    Eigen::MatrixXd data(5, 3);
    data << 2.5, 2.4, 1.1,
            0.5, 0.7, 3.2,
            2.2, 2.9, 0.5,
            1.9, 2.2, 2.0,
            3.1, 3.0, 1.1;

    std::cout << "Original Data (5x3):\n" << data << "\n\n";

    // Initialize PCA to reduce from 3 dimensions to 2
    PCA pca(2);
    Eigen::MatrixXd reduced_data = pca.fit_transform(data);

    std::cout << "Reduced Data (5x2):\n" << reduced_data << "\n";

    return 0;
}
