/******************************************************************************
 * Copyright (c) 2014, Hobu Inc. (hobu@hobu.co)
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following
 * conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of the Martin Isenburg or Iowa Department
 *       of Natural Resources nor the names of its contributors may be
 *       used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************************/

#include <pdal/util/IStream.hpp>
#include <pdal/util/FileUtils.hpp>

#include "FixCOPCSRS.hpp"
#include "Lasdump.hpp"

int main(int argc, char *argv[])
{
    std::deque<std::string> args;

    for (int i = 1; i < argc; ++i)
        args.push_back(argv[i]);
    pdal::lasdump::Dumper d(args);
    if (!d.error())
        d.dump();
}

namespace
{

void usage(const std::string& err = "")
{
    if (!err.empty())
        std::cerr << "Error: " << err << "\n";
    std::cerr << "Usage: lasdump input.laz output.laz srs.wkt\n";
}

}


namespace pdal
{
namespace lasdump
{

void Dumper::dump()
{
    ILeStream in(m_COPCfilename);

    std::string srsDef = pdal::FileUtils::readFileIntoString(m_SRSfilename);


    if (!in.good())
    {
        std::cerr << "Error: Couldn't open file \"" << m_COPCfilename << "\".\n";
        m_error = -1;
        return;
    }


    // copy the file
    //
    std::ifstream source(m_COPCfilename, std::ios::binary);
    std::ofstream dest(m_outputfilename, std::ios::binary);

    dest << source.rdbuf();
    source.close();
    dest.close();
    in.close();
    in.open(m_outputfilename);

    try {
        in >> m_header;
    }
    catch (Exception& ex)
    {
        std::cerr << "Error: " << ex << "\n";
        m_error = -1;
        return;
    }


//     *m_out << m_header;

    //
    in.seek(m_header.vlrOffset());
    for (uint32_t i = 0; i < m_header.vlrCount(); ++i)
    {
        Vlr vlr;

        std::streampos position = in.position();
        in >> vlr;
//         std::streampos position = in.position();
        if (vlr.matches("LASF_Projection", 2112))
        {
            in.close();
            std::streampos defPos = (int) position + 2 + 16 + 2 + 2 + 32; // VLR Header size
            uint64_t len = vlr.dataLen();
            std::ofstream out(m_outputfilename, std::ios::binary | std::ios::in | std::ios::out );
            out.seekp(defPos);
            *m_out << "writing srs def at " << (int)defPos << std::endl;
            out << srsDef ;
            out.flush();
            *m_out << "wrote srs def to " << (int)out.tellp() << std::endl;
            out.close();

//             *m_out << "SRS VLR is at " << (int) position << " with length " << len << std::endl;
//             *m_out << vlr;
//             *m_out << srsDef << std::endl;
        }
    }



    return;
}

int Dumper::processArgs(std::deque<std::string> args)
{

//         m_fout.open(args[1]);
//         m_out = &m_fout;
//         if (!*m_out)
//         {
//             usage("Couldn't open output file.");
//             return -1;
//         }
//         args.pop_front();
//     }
//     if (args.size() != 2)
//     {
//         usage();
//         return -1;
//     }

    m_COPCfilename = args[0];
    m_SRSfilename = args[2];
    m_outputfilename = args[1];
    return 0;
}


} // namespace lasdump
} // namespace pdal

