import subprocess

expected_samples = 3
normal_command = [
    "sudo", "/home/linaro/imu-read", str(expected_samples), "--csv",
]
normal_result = subprocess.run(normal_command, capture_output=True, text=True)

lines = normal_result.stdout.splitlines()
sample_lines = lines[1:]
actual_samples = len(sample_lines)
previous_ms = None
timestamps_increase = True

for line in sample_lines:
    try:
        current_ms = int(line.split(",")[0])
    except ValueError:
        timestamps_increase = False
        break

    if previous_ms is not None and current_ms <= previous_ms:
        timestamps_increase = False

    previous_ms = current_ms

normal_pass = (
    normal_result.returncode == 0
    and actual_samples == expected_samples
    and timestamps_increase
)
print("normal =", "PASS" if normal_pass else "FAIL")
print("samples =", actual_samples)
print("timestamps_increase =", timestamps_increase)

timeout_command = [
    "sudo", "strace",
    "-e", "trace=ioctl",
    "-P", "/dev/i2c-3",
    "-e", "inject=ioctl:error=ETIMEDOUT:when=4",
    "/home/linaro/imu-read",
]
timeout_result = subprocess.run(timeout_command, capture_output=True, text=True)

injection_seen = (
    "ETIMEDOUT" in timeout_result.stderr
    and "(INJECTED)" in timeout_result.stderr
)
expected_error = "IMU read failed: read IMU registers: Connection timed out"
error_seen = expected_error in timeout_result.stderr

timeout_pass = timeout_result.returncode == 1 and injection_seen and error_seen
print("injected_timeout =", "PASS" if timeout_pass else "FAIL")
print("timeout_exit =", timeout_result.returncode)

overall_pass = normal_pass and timeout_pass
print("overall =", "PASS" if overall_pass else "FAIL")
raise SystemExit(0 if overall_pass else 1)
