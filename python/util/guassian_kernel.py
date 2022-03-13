#from math import exp

#def gaussian(x, mu, sigma):
#  return exp( -(((x-mu)/(sigma))**2)/2.0 )

#kernel_radius = 5
#sigma = kernel_radius/2. # for [-2*sigma, 2*sigma]

# compute the actual kernel elements
#hkernel = [gaussian(x, kernel_radius, sigma) for x in range(2*kernel_radius+1)]

# Normalize
#kernelsum = sum(hkernel)
#kernel1d = [x/kernelsum for x in hkernel]

#print( ["%.6f" % x for x in kernel1d] )

from math import exp

def gaussian(x, mu, sigma):
  return exp( -(((x-mu)/(sigma))**2)/2.0 )

#kernel_height, kernel_width = 7, 7
kernel_radius = 50 # for an 7x7 filter
sigma = kernel_radius/3. # for [-2*sigma, 2*sigma]

# compute the actual kernel elements
hkernel = [gaussian(x, kernel_radius, sigma) for x in range(2*kernel_radius+1)]
vkernel = [x for x in hkernel]
kernel2d = [[xh*xv for xh in hkernel] for xv in vkernel]

# normalize the kernel elements
#kernelsum = sum([sum(row) for row in kernel2d])
#kernel2d = [[x/kernelsum for x in row] for row in kernel2d]

kernelsum = sum(kernel2d[0])
kernel1d = [x/kernelsum for x in kernel2d[0]]

for line in kernel2d:
  print( ["%.6f" % x for x in line] )

length = len(kernel1d)
middle_index = length//2
print( middle_index + 1 )
print( [x for x in kernel1d[middle_index:]] )




