import os

instancias = [
	"MANN_a9",
	"brock200_2",
	"brock200_3",
	"brock200_4",
	"c-fat200-1",
	"c-fat200-2",
	"c-fat500-1",
	"c-fat500-2",
	"hamming6-2",
	"hamming6-4",
	"johnson8-2-4",
	"johnson8-4-4",
	"johnson16-2-4",
	"keller4",
	"p_hat300-1",
	"p_hat500-1",
	"p_hat700-1",
	"p_hat1000-1",
	"sanr200_0.7",
	"sanr400_0.5"
]


os.system("make")
os.system("rm -d -R output")
os.system("mkdir output")

for inst in instancias:
	for alg in ['LOAD', 'STACK', 'SERIAL']:
		os.system("mkdir output/%s" % inst)
		for k in [2, 3]:
			os.system("mkdir output/{0}/{1}".format(inst, k))
			if alg == 'SERIAL':
				print ("./{2} 3600 {1} input/{0}.clq >> output/{0}/{1}/{2}.out".format(inst, k, alg))		
				os.system("./{2} 3600 {1} input/{0}.clq >> output/{0}/{1}/{2}.out".format(inst, k, alg))
			else:
				for nt in [2, 4, 8, 12, 16]:
					print ("./{3} 3600 {0} {2} input/{1}.clq >> output/{1}/{2}/{3}.out".format(nt, inst, k, alg))			
					os.system("./{3} 3600 {0} {2} input/{1}.clq >> output/{1}/{2}/{3}.out".format(nt, inst, k, alg))

