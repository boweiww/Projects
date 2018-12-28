import numpy as np
import struct



def loadImageSet(filename):
    binfile = open(filename, 'rb')
    buffers = binfile.read()

    offset = struct.calcsize('>IIII')
    imgNum = 60000
    width = 28
    height = 28

    bits = imgNum * width * height
    bitsString = '>' + str(bits) + 'B'

    imgs = struct.unpack_from(bitsString, buffers, offset)

    binfile.close()
    values = np.reshape(imgs, [imgNum, width * height])
    set = np.array_split(values, 6)
    valid = set[5]
    train = set[0]
    for i in range(len(set) - 2):
        train = np.vstack((train, set[i + 1]))
    return train,valid


def loadLabelSet(filename):
    binfile = open(filename, 'rb')
    buffers = binfile.read()

    labelNum = 60000
    offset = struct.calcsize('>II')
    numString = '>' + str(labelNum) + "B"
    labels = struct.unpack_from(numString, buffers, offset)

    binfile.close()
    values = np.reshape(labels, [labelNum])
    set = np.array_split(values, 6)
    valid = set[5]
    train = set[0]
    for i in range(len(set) - 2):
        train = np.hstack((train, set[i + 1]))
    return train,valid

def loadTestSet(filename):
    binfile = open(filename, 'rb')
    buffers = binfile.read()

    offset = struct.calcsize('>IIII')
    imgNum = 10000
    width = 28
    height = 28
    bits = imgNum * width * height
    bitsString = '>' + str(bits) + 'B'

    imgs = struct.unpack_from(bitsString, buffers, offset)

    binfile.close()
    values = np.reshape(imgs, [imgNum, width * height])

    return values


def loadTestLabel(filename):
    binfile = open(filename, 'rb')
    buffers = binfile.read()

    labelNum = 10000
    offset = struct.calcsize('>II')
    numString = '>' + str(labelNum) + "B"
    labels = struct.unpack_from(numString, buffers, offset)

    binfile.close()
    values = np.reshape(labels, [labelNum])

    return values

if __name__ == "__main__":
    # 1 and 2 are for train and valid dataset
    file1 = 'mnist/train-images.idx3-ubyte'
    file2 = 'mnist/train-labels.idx1-ubyte'


    # 3 and 4 are for test set
    file3 = 'mnist/t10k-images.idx3-ubyte'
    file4 = 'mnist/t10k-labels.idx1-ubyte'

    trainset,validset = loadImageSet(file1)
    print(trainset.shape)
    print(validset.shape)
    trainresult, validresult = loadLabelSet(file2)
    print(trainresult.shape)
    print(validresult.shape)
    testset = loadTestSet(file3)
    print(testset.shape)
    testlabel = loadTestLabel(file4)
    print(testlabel.shape)



