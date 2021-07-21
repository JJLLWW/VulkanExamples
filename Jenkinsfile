pipeline {
    agent any 
    stages {
    stage('Run') { 
            steps {
                script {
                    OUTPUT = sh(returnStdout: true, script: "ls -la")
                    echo "$OUTPUT"
                }
            }
        }
    }
}