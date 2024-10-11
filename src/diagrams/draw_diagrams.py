import matplotlib.pyplot as plt
import numpy as np
import os

# File paths
serial_path = '../results/serial_results.txt'
mutex_path = '../results/mutex_results.txt'
rw_lock_path = '../results/rw_lock_results.txt'

# Define cases // (case, mMember, mInsert, mDelete)
cases = [
    ('Case 1', 0.99, 0.005, 0.005),
    ('Case 2', 0.9, 0.05, 0.05),
    ('Case 3', 0.5, 0.25, 0.25)
]

# Define methods
methods = ['Serial', 'Mutex', 'RWLock']

def getData():
    data = {'Case 1': [], 'Case 2': [], 'Case 3': []}

    if os.path.isfile(serial_path):
        with open(serial_path, 'r') as f:
            lines = f.readlines()
            for line in lines[::-1]:
                extract_data(line, "Serial", data)
    else:
        print(f'File {serial_path} not found.')
    
    if os.path.isfile(mutex_path):
        with open(mutex_path, 'r') as f:
            lines = f.readlines()
            for line in lines[::-1]:
                extract_data(line, "Mutex", data)
    else:
        print(f'File {mutex_path} not found.')
    
    if os.path.isfile(rw_lock_path):
        with open(rw_lock_path, 'r') as f:
            lines = f.readlines()
            for line in lines[::-1]:
                extract_data(line, "RWLock",data)
    else:
        print(f'File {rw_lock_path} not found.')

    return data

def extract_data(line, method_name, data):
    if not line.strip():
        return
    parts = line.strip().split('|')
    result = {}
    for part in parts:
        key_value = part.strip().split(':')
        if len(key_value) != 2:
            continue
        key = key_value[0].strip()
        value = key_value[1].strip().strip('%')
        try:
            if '.' in value:
                result[key] = float(value)
            else:
                result[key] = int(value)
        except ValueError:
            result[key] = value

    for case in cases:
        case_name, mMember, mInsert, mDelete = case
        # find case
        if mMember == result['mMember'] and mInsert == result['mInsert'] and mDelete == result['mDelete']:
            # If data found in case, don't add it again. wee need only the last result
            # check for thread count and method, mMemeber, mInsert, mDelete are already same
            for case_data in data[case_name]:
                if result['threadCount'] == case_data['threadCount'] and case_data['method'] == method_name:
                    break
            else:
                result['method'] = method_name
                data[case_name].append(result)
            break

def plot_graph(case, mMember, mInsert, mDelete, data):
    print(f'Plotting graph for {case} with mMember: {mMember}, mInsert: {mInsert}, mDelete: {mDelete}')
    method_data = data[case]

   # Get unique thread counts
    thread_counts = sorted(set(entry['threadCount'] for entry in method_data))
    x = np.arange(len(thread_counts))

    width = 0.2

    fig, ax = plt.subplots()

    for i, method in enumerate(methods):
        means = []
        for tc in thread_counts:
            # Find the entry for this method and thread count
            entries = [entry for entry in method_data if entry['method'] == method and entry['threadCount'] == tc]
            if entries:
                mean = entries[0]['mean']
            else:
                mean = 0 
            means.append(mean)
        offset = (i - 1) * width 
        rects = ax.bar(x + offset, means, width, label=method)

    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_xlabel('Thread Count')
    ax.set_ylabel('Mean Time (µs)')
    ax.set_title(f'Performance vs Thread Count \n{case}: (mMember: {mMember}, mInsert: {mInsert}, mDelete: {mDelete})')
    ax.set_xticks(x)
    ax.set_xticklabels(thread_counts)
    ax.legend()

    fig.tight_layout()

    # Save the plot
    plt.savefig(f'{case}.png')
    print(f'Plot saved as {case}.png in the graphs folder.')
    plt.show()

def main():
    data = getData()
    for case, mMember_value, mInsert_value, mDelete_value in cases:
        plot_graph(case, mMember_value, mInsert_value, mDelete_value, data)

if __name__ == "__main__":
    main()
