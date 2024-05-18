import random
import matplotlib.pyplot as plt
import math

def gui(centroids: list[list], points: list[list]):
    """Show the clusters in a gui."""

    def asign(centroids, points):

        output = [[] for _ in range(len(centroids))]

        for point in points:
            index = asign_centroids(centroids, point)
            output[index] += [point]

        return output

    fig, ax = plt.subplots()

    for i, (centroid, point) in enumerate(zip(centroids, asign(centroids, points))):

        x, y = zip(*point)
        ax.scatter(x, y, color=f'C{i}')
        ax.scatter(*centroid, color=f'C{i}', s=100, marker='o', edgecolors='black', linewidths=2)

    ax.legend()
    plt.show()


def random_centroids(count_k: int, dimension: int, range_: tuple) -> list[list]:
    """
    generates a random number of centroids

    :param int count_k: number of centroids
    :param int dimension: dimensions of vectors

    :return list[list[float, ...]]: list of centroids
    """

    return [list(random.randint(*range_) for _ in range(dimension)) for _ in range(count_k)]


def empty_centroids(count_k: int, dimension: int) -> list[list]:
    """
    generates a number of centroids

    :param int count_k: number of centroids
    :param int dimension: dimensions of vectors

    :return list[list[float, ...]]: list of centroids
    """

    return [list(0 for _ in range(dimension)) for _ in range(count_k)]


def calc_distance(point_a: tuple[float, ...], point_b: tuple[float, ...]) -> float:
    """
    calc the distance between two points

    :param tuple[float, ...] point_a:
    :param tuple[float, ...] point_b:

    :return float:
    """

    distance: float = 0

    for dimension, point in enumerate(point_a):
        distance += (point - point_b[dimension]) ** 2

    return distance**.5


def asign_centroids(
    centroids: list[tuple[float, ...]], point: tuple[float, ...]
) -> int:
    """
    returns the closed centroid to point

    :param list[tuple[float, ...]] centroids: list of all centroids
    :param tuple[float, ...] point: point to calc the closed

    :return int: index of centroid in list of centroids
    """

    best: int = 0

    for index, _ in enumerate(centroids):
        if calc_distance(centroids[index], point) < calc_distance(
            centroids[best], point
        ):
            best = index

    return best


def k_means(points,k: int):
    d = 2
    
    points2 = list(points).copy()
    centroids_old = []
    centroids_old.append(points2.pop(random.randint(0, len(points2)-1)))
    #centroids_old.append(points2.pop(random.randint(0, len(points2))))

    try: 
        for p in range(k-1):
            i = 0
            maxi_idx = 0
            maxi_val = 0
            for point in points2:
                
                if sum([math.dist(point, c) for c in centroids_old]) > maxi_val:
                    maxi_val = sum([math.dist(point, c) for c in centroids_old])
                    maxi_idx = i
                        
                i += 1
            centroids_old.append(points2.pop(maxi_idx))

    except IndexError:
        pass

    i: int = 0

    while True:
        centroids_new: list = empty_centroids(k, d)
        centroids_counter: list = list(0 for _ in range(k))

        # Add share of average

        for point in points:
            centroid: int = asign_centroids(centroids_old, point)
            centroids_counter[centroid] += 1

            for dimension in range(d):
                centroids_new[centroid][dimension] += point[dimension]

        # Calc centroid average

        for centroid in range(k):
            for dimension in range(d):
                if centroids_counter[centroid] > 0:
                    centroids_new[centroid][dimension] /= centroids_counter[centroid]

        if centroids_old == centroids_new:
            break

        centroids_old = centroids_new.copy()
        i += 1

        print(f"{centroids_new = }")

    # merge near centroids
    
    MERGE_THRESHOLD = 60
    centroids = []
    
    for i in range(3):
        for t in centroids_old:
            for t2 in centroids_old:
                if t != t2:
                    #print(t,t2,math.dist(t, t2))
                    if math.dist(t, t2) <= MERGE_THRESHOLD:
                        if not ((t[0]+t2[0])/2, (t[1]+t2[1])/2) in centroids:
                            centroids.append(((t[0]+t2[0])/2, (t[1]+t2[1])/2))
                        break
            else:
                centroids.append(t)
        centroids_old = centroids
        centroids = []

    #print(centroids_old)
    return centroids_old

