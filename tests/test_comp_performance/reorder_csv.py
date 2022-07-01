import csv

IN_FILE = "test_results/5_chest_cnn_test_averages_FULL.csv"
OUT_FILE = "test_results/5_chest_cnn_test_averages_FULL_reordered.csv"
NUM_PERFORMANCE_METRICS = 8
with open(IN_FILE, newline="") as csvfile, open(OUT_FILE, "w") as csv_file_out:
    csv_reader = csv.reader(csvfile, delimiter=",", quotechar='"')
    csv_writer = csv.writer(csv_file_out, delimiter=",", quotechar='"')
    is_first_row = True
    performance_metrics_by_num_filters_and_config = dict()
    configs = []
    for row in csv_reader:
        if is_first_row:
            headers = row
            is_first_row = False
            performance_headers = row[11 : 11 + NUM_PERFORMANCE_METRICS + 1]
        else:
            lib = row[0]
            batch = int(row[1])
            scale = int(row[7])
            num_filters = int(row[9])
            performance_metrics = row[11 : 11 + NUM_PERFORMANCE_METRICS + 1]
            if (lib == "HELib" and scale < 10) or (lib == "SEAL" and scale < 30):
                config = 1
            else:
                config = 2
            config_name = "{}_{}K_{}".format(lib, batch // 1024, config)
            if config_name not in configs:
                configs.append(config_name)
            if num_filters not in performance_metrics_by_num_filters_and_config:
                performance_metrics_by_num_filters_and_config[num_filters] = dict()
            performance_metrics_by_num_filters_and_config[num_filters][
                config_name
            ] = performance_metrics
    out_header_row_1 = [""]
    for perf_metric_index in range(NUM_PERFORMANCE_METRICS):
        out_header_row_1.append(performance_headers[perf_metric_index])
        for i in range(3):
            out_header_row_1.append("")
    csv_writer.writerow(out_header_row_1)

    out_header_row_2 = ["config x num_filters"]
    for perf_metric_index in range(NUM_PERFORMANCE_METRICS):
        for num_filters in range(1, 5):
            out_header_row_2.append(num_filters)
    csv_writer.writerow(out_header_row_2)

    for config in configs:
        row = [config]
        for perf_metric_index in range(NUM_PERFORMANCE_METRICS):
            for num_filters in range(1, 5):
                if config in performance_metrics_by_num_filters_and_config[num_filters]:
                    row.append(
                        performance_metrics_by_num_filters_and_config[num_filters][
                            config
                        ][perf_metric_index]
                    )
                else:
                    row.append("")
        csv_writer.writerow(row)
