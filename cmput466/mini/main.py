from dataload import *
from NeuralNet import *
import sys
import numpy as np
from scipy import stats



def vectorize(a):
    cases = np.zeros((10, 1))
    cases[a] = 1.0
    return cases

def sigmoid(a):
        return 1.0 / (1.0 + np.exp(-a))

def SGD(traindata, trainresult,validdata,validresult, learning_rate, epochs):
    # dataMat = mat(dataArray)  # size:m*n
    # labelMat = mat(labelArray)  # size:m*1
    # m, n = traindata.shape
    m = len(traindata)
    n = 784
    nums = np.arange(m)
    weight = np.zeros((10, n))
    accuracy = []
    for i in range(epochs):
        random.shuffle(nums)
        for j in range (m):
            h = sigmoid(weight.dot(traindata[nums[j]]))
            # print(h.shape)
            error = trainresult[nums[j]] - h  # size:m*1
            weight = weight + learning_rate * traindata[nums[j]].transpose() * error
        validation_results = [(np.argmax(sigmoid(weight.dot(x))) == y) for x, y in zip(validdata,validresult)]
        a = sum(validation_results)/ 100.0
        print("Epoch: " + str(i + 1) + "     accuracy: " + str(a) + "%.")
        accuracy.append(a)

    return weight,accuracy

if __name__ == "__main__":

    file1 = 'mnist/train-images.idx3-ubyte'
    file2 = 'mnist/train-labels.idx1-ubyte'

    # 3 and 4 are for test set
    file3 = 'mnist/t10k-images.idx3-ubyte'
    file4 = 'mnist/t10k-labels.idx1-ubyte'

    trainset,validset = loadImageSet(file1)
    trainresult, validresult = loadLabelSet(file2)
    testset = loadTestSet(file3)
    testlabel = loadTestLabel(file4)
    # normalize
    trainset = np.true_divide(trainset, 255)
    validset = np.true_divide(validset, 255)
    testset = np.true_divide(testset, 255)


    trainset = [np.reshape(x, (784, 1)) for x in trainset]
    validset = [np.reshape(x, (784, 1)) for x in validset]
    testset = [np.reshape(x, (784, 1)) for x in testset]

    trainresult = [vectorize(y) for y in trainresult]
    # validresult = [vectorize(y) for y in validresult]
    # testlabel = [vectorize(y) for y in testlabel]

    layers = [784,80,10]
    learning_rate = 0.01
    mini_batch_size = 1
    epochs = 2
    # simple logit regression
    print('logistic regression:')
    weight,accuracy = SGD(trainset,trainresult,validset,validresult,learning_rate,epochs)
    validation_results = [(np.argmax(sigmoid(weight.dot(x))) == y) for x, y in zip(testset, testlabel)]
    print("Test Accuracy: " + str(sum(validation_results) / 100.0) + "%")
    # np.save('logit.npy', np.array(accuracy))
    accuracy1 = accuracy

    # SGD neural network
    print("stochestic gradient descent neural network:")
    nn = NeuralNetwork(layers, learning_rate, mini_batch_size, epochs)
    accuracy = nn.learn(trainset,trainresult,validset,validresult)

    print("Test Accuracy: " + str(nn.validate(testset,testlabel) / 100.0 )+ "%")
    # np.save('sgd.npy', np.array(accuracy))
    accuracy2 = accuracy

    # mini-batch neural network
    print("mini-batch gradient descent neural network:")
    mini_batch_size = 4
    nn = NeuralNetwork(layers, learning_rate, mini_batch_size, epochs)
    accuracy = nn.learn(trainset,trainresult,validset,validresult)

    print("Test Accuracy: " + str(nn.validate(testset,testlabel) / 100.0 )+ "%")
    # np.save('minibatch.npy', np.array(accuracy))
    accuracy3 = accuracy
    with open("output.txt", "w") as file:
        file.write(str(accuracy1))
        file.write(str(accuracy2))
        file.write(str(accuracy3))
    file.close()

    # np.savetxt('output', (accuracy1,accuracy2,accuracy3))


    #save the model
    #nn.save()