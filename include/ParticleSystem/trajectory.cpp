#include <ParticleSystem/trajectory.h>

void Trajectory::takeReading(ParticleSystem & p){
    std::vector<State> s(p.size(),State(0.0,0.0,0.0,0.0));
    for (int i = 0; i < p.size(); i++){
        s[i].x = p.state[i*3];
        s[i].y = p.state[i*3+1];
        s[i].theta = p.state[i*3+2];
        s[i].radius = p.radius;
        size += 1;
    }
    trajectory.push_back(s);
    parameters.push_back(
        Parameters(
            p.coefficientOfRestitution,
            p.size(),
            p.orderParameter(),
            speed,
            p.getParameter(ParticleSystem::Parameter::RepelDistance),
            p.getParameter(ParticleSystem::Parameter::AlignDistance),
            p.getParameter(ParticleSystem::Parameter::AttractDistance),
            p.getParameter(ParticleSystem::Parameter::Diffusion),
            p.getParameter(ParticleSystem::Parameter::Speed),
            p.getParameter(ParticleSystem::Parameter::ResponseRate),
            p.getParameter(ParticleSystem::Parameter::BlindAngle)
        )
    );

    if (size*(sizeof(State)) > CACHE_SIZE_BYTES){
        save();
        ioMode = std::ios_base::app;
        clear();
    }
}

void Trajectory::save(){

    #if WINDOWS
        // threading needs to be handled differently
        //   apparently
        threadedSave(trajectory);
    #else
        std::thread job(&Trajectory::threadedSave,this,this->trajectory);
        job.detach();
    #endif

}

void Trajectory::threadedSave(std::vector<std::vector<State>> trajectory){
    std::ofstream out(file,ioMode);
    std::ofstream outParam("parameters-"+file,ioMode);
    if (out.is_open() && outParam.is_open()){
        for (int t = 0; t < trajectory.size(); t++){
            for (int i = 0; i < trajectory[t].size(); i++){
                out << trajectory[t][i].x << ", "
                    << trajectory[t][i].y << ", "
                    << trajectory[t][i].theta << ", "
                    << trajectory[t][i].radius << "\n";
            }
            out << "\n";
            outParam << parameters[t].restitution << ","
                     << parameters[t].nParticles << ","
                     << parameters[t].order << ","
                     << parameters[t].speed << ","
                     << parameters[t].rd << ","
                     << parameters[t].ad << ","
                     << parameters[t].attrd << ","
                     << parameters[t].diff << ","
                     << parameters[t].v0 << ","
                     << parameters[t].blind << ","
                     << parameters[t].response << "\n";

        }
        out.close(); 
        outParam.close();
    }
    else{
        throw IOException("Cannot open file "+file);
    }
}