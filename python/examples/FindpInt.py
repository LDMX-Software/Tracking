#%%
import numpy as np
import matplotlib.pyplot as plt
import ROOT as Root

def sum_until(lst, ind):
    return sum(lst[:ind])/sum(lst)


def makeBins(th1obj):
    # binsList is a list of bin_content 
    # bin_content is a list of bin values for the respective th1obj in th1objList
    bin_content = []
    for i in range(1, th1obj.GetNbinsX()+1):
        bin_content.append(th1obj.GetBinContent(i))
    return bin_content


def vecDeriv(xarr, yarr):
    xarr, yarr = np.array(xarr), np.array(yarr)
    y_p = yarr[1:] - yarr[:-1]
    x_p = xarr[1:] - xarr[:-1]
    return y_p/x_p

# def makeBinsList(th1objList):
#     # binsList is a list of bin_content 
#     # bin_content is a list of bin values for the respective th1obj in th1objList
#     binsList = []
#     for i in range(th1objList):
#         th1obj = th1objList[i]
#         bin_content = []
#         for i in range(1, th1obj.GetNbinsX()+1):
#             bin_content.append(th1obj.GetBinContent(i))
#         binsList.append(bin_content)
#     return binsList


INT_E = 1.2     # GeV

# smearing_list = [
#     0.005,
#     # 0.007,
#     # 0.010, 
#     # 0.013,
#     # 0.015,
#     0.018,
#     # 0.020,
#     # 0.023,
#     # 0.025,
#     0.028,
#     # 0.030, 
#     # 0.033,
#     0.035,
#     # 0.038,
# ]

smearing_list = [
    0.004,
    0.006,
    0.008,
    0.010,
    0.012,
    0.014,
    0.016,
    0.018,
    0.020,
    0.022,
    0.024,
    0.026,
    0.028,
    0.030,
    0.032,
    0.034,
    0.036,
    0.038,
]

# fig = plt.figure()
# h1 = fig.add_subplot(1,1,1)
fig, h1 = plt.subplots(1,1)

path = "../../../single_e_reco/"

# file_list = [
#     "Recoil_TrackFinder_u0.005_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.007_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.010_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.013_v0.1_n50000.root",
#     "Recoil_TrackFinder_u0.015_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.018_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.020_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.023_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.025_v0.1_n50000.root",
#     "Recoil_TrackFinder_u0.028_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.030_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.033_v0.1_n50000.root",
#     # "Recoil_TrackFinder_u0.035_v0.1_n50000.root",
#     "Recoil_TrackFinder_u0.038_v0.1_n50000.root",
# ]

file_list = ["Recoil_TrackFinder_Ecal_u{0:0.3f}_v0.1_n50000.root".format(item) for item in smearing_list]


summed_list = []
for f in range(len(file_list)):
    file = file_list[f]
    a = Root.TFile.Open(path+file)
    a1 = a.Get("p")

    rebin_num = 10
    a1 = a1.Rebin(rebin_num)

    binnxs = [a1.GetXaxis().GetBinLowEdge(1)]
    # print(binnxs)

    bin_contenta = makeBins(a1)


    for i in range(1,a1.GetNbinsX()+1):
        binnxs.append(a1.GetXaxis().GetBinUpEdge(i))

    binmax = len(bin_contenta)
    bin_x = np.arange(0, binmax/100, 1/100) * rebin_num

    # label is uSmearing in mm

    # h1.plot(bin_x, bin_contenta, label=smearing_list[f])
    # using step, trying stairs
    plt.step(
        bin_x, bin_contenta, 
        label=smearing_list[f],
        )

    # h1.set_ylim(0,500)

    # --------

    ind = 0
    while bin_x[ind] < INT_E:
        ind += 1

    summed_a = sum_until(bin_contenta, ind)
    summed_list.append(summed_a)

    


h1.legend(title='uSmearing (mm)')
h1.set_xlabel('p (GeV)')
h1.set_ylabel('events')
h1.set_title('(prelim) Effects of uSmearing on Reconstructed Momenta')
plt.tight_layout()
plt.show()
plt.savefig('new.png',facecolor=fig.get_facecolor(),         
            edgecolor=fig.get_edgecolor())



summed_list = np.asarray(summed_list)
summed_list = summed_list*100

fig, ax1 = plt.subplots()
ax2 = ax1.twinx()

ax1.plot(smearing_list, summed_list)
ax1.scatter(smearing_list, summed_list)
ax2.plot(smearing_list[1:], vecDeriv(smearing_list, summed_list), c='k')
# plt.stem(smearing_list, summed_list, linefmt='--', basefmt='None')
ax1.set_xlabel('uSmearing (mm)')
ax1.set_ylabel('% Counts Below 1.2 GeV')
ax2.set_ylabel('First derivative')
plt.title('Smearing vs. Counts below 1.2 GeV')
plt.show()

#%%

totals = [
    13128.0,
    12695.0,
    12552.0,
    12375.0,
]

plt.bar(list(range(0,4)), totals, width=0.5)

plt.show()
