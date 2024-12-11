#include "header/ipc"

#define exec(env) ((env) ? execvp : execv)

#define FAIL(r) { this->fail = true; this->reason = (r); return; }

Process::~Process() {
    this->Kill();
}

void Process::_wait_proc() {
    int r;
    while (!(r = waitpid(this->pid, &this->_stat, WNOHANG)));

    this->_ext = std::chrono::system_clock::now();

    if (r == -1) FAIL("Failed on wait. (" + (std::string)strerror(errno) + ")");

    if      (WIFEXITED  (this->_stat)) _exc = WEXITSTATUS(this->_stat);
    else if (WIFSIGNALED(this->_stat)) _sig = WTERMSIG   (this->_stat);
    else FAIL("Unknown error on status of child process.");
}

void Process::_close_fd() {
    if (this->info.RedirectStandardInput ) { delete _inp; close(this->in [0]); close(this->in [1]); }
    if (this->info.RedirectStandardOutput) { delete _out; close(this->out[0]); close(this->out[1]); }
    if (this->info.RedirectStandardError ) { delete _err; close(this->err[0]); close(this->err[1]); }
}

void Process::Kill() {
    int r;
    if (!this->HasExited() && (r = kill(this->pid, SIGKILL)))
        FAIL("Failed to kill child process. (" + (std::string)strerror(errno) + ")");
    if (this->wait_thread.joinable()) this->wait_thread.join();

    this->_close_fd();
}

void Process::Start() {
    if (this->info.RedirectStandardInput  && pipe(this->in ) < 0) FAIL("Failed to open StandardInput." );
    if (this->info.RedirectStandardOutput && pipe(this->out) < 0) FAIL("Failed to open StandardOutput.");
    if (this->info.RedirectStandardError  && pipe(this->err) < 0) FAIL("Failed to open StandardError." );

    if (this->info.RedirectStandardInput ) { this->_pin  = Pipe(this->in ); this->_inp = new std::ofdstream(this->in [1]); }
    if (this->info.RedirectStandardOutput) { this->_pout = Pipe(this->out); this->_out = new std::ifdstream(this->out[0]); }
    if (this->info.RedirectStandardError ) { this->_perr = Pipe(this->err); this->_err = new std::ifdstream(this->err[0]); }

    if (this->info.RedirectStandardInput && this->info.RedirectStandardOutput)
    { this->_out->tie(_inp); }
    
    this->pid = fork();

    if (pid < 0) FAIL("Failed to create process.");
    if (pid == 0) {
        if (this->info.RedirectStandardInput )
        {dup2(in [0], STDIN_FILENO ); close(this->in [0]); close(this->in [1]); }
        if (this->info.RedirectStandardOutput)
        {dup2(out[1], STDOUT_FILENO); close(this->out[0]); close(this->out[1]); }
        if (this->info.RedirectStandardError )
        {dup2(err[1], STDERR_FILENO); close(this->err[0]); close(this->err[1]); }

        const char* args[this->info.Arguments.size() + 2];

        args[0] = this->info.ProcessName.value_or(this->info.ProcessPath).c_str();
        args[this->info.Arguments.size() + 1] = (char*)0;

        for (int i = 1; i < this->info.Arguments.size() + 1; i++)
            args[i] = this->info.Arguments[i - 1].c_str();
        
        exec(this->info.UsePathEnv)(this->info.ProcessPath.c_str(), (char* const*)(args));

        _close_fd();
        FAIL("Failed to start process.");
    }

    this->_stt        = std::chrono::system_clock::now();
    this->wait_thread = std::thread(&Process::_wait_proc, this);

    if (this->info.RedirectStandardInput ) close(this->in [0]);
    if (this->info.RedirectStandardOutput) close(this->out[1]);
    if (this->info.RedirectStandardError ) close(this->err[1]);
}
