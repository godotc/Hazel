//
// Created by nono on 9/23/23.
//

#ifndef HAZEL_TIMESTEP_H
#define HAZEL_TIMESTEP_H

namespace hazel {

class Timestep
{
  public:
    Timestep(float time = 0.f) : m_Time(time) {}

    operator float() const { return m_Time; }

    float GetSeconds() const { return m_Time; }
    float GetMiliseconds() const { return m_Time * 1000.f; }

  private:
    float m_Time;
};

} // namespace hazel

#endif // HAZEL_TIMESTEP_H
